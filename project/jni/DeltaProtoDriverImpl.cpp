#include "DeltaProtoDriverImpl.h"
#include <jni.h>

#include <stdio.h>		
#include "pubSysCls.h"	
#include <thread>

using namespace sFnd;

#define ACC_LIM_RPM_PER_SEC	600
#define VEL_LIM_RPM			1500
#define TIME_TILL_TIMEOUT	30000	//The timeout used for homing(ms)

class DeltaProtoDriverObj {
	SysManager *m_sysManager;
	IPort *m_iPort;
	INode *m_node_x;			// The ClearPath-SC for this axis
	INode *m_node_y;			// The ClearPath-SC for this axis
	int m_var;
	int m_portNum;
public:
	// Constructor
	DeltaProtoDriverObj();
	void openPort( int portNum );
	void home();
	void closePort();
	void moveTo( int x, int y, int acc, int vel, int release);
	void moveToDelay( int x, int y, int acc, int vel, int release, int delayX, int delayY);
	void moveToX( int x, int acc, int vel, int release);
	void moveToY( int y, int acc, int vel, int release);
	void release( int rel);
	double getPositionX();
	double getPositionY();
private:
	void homeAllNodes(IPort *myPort);
};


#ifdef __cplusplus
        extern "C" {
#endif

	void * 	createDriver(int portNum) { 
		DeltaProtoDriverObj* dpd= new DeltaProtoDriverObj(); 
		dpd->openPort(portNum);
		return dpd;
	}
	void home(void * p) { 
		((DeltaProtoDriverObj*)p)->home();
	}
	void    freeDriver(void * p) { 
		((DeltaProtoDriverObj*)p)->closePort();
		delete p; 
	}
	void moveTo(void * p, int x, int y, int acc, int vel, int release) { 
		((DeltaProtoDriverObj*)p)->moveTo(x, y, acc, vel,release); 
	}
	void moveToX(void * p, int x, int acc, int vel, int release) { 
		((DeltaProtoDriverObj*)p)->moveToX(x, acc, vel,release); 
	}
	void moveToY(void * p, int y, int acc, int vel, int release) { 
		((DeltaProtoDriverObj*)p)->moveToY(y, acc, vel,release); 
	}
	void release(void * p, int rel) { 
		((DeltaProtoDriverObj*)p)->release(rel); 
	}
	double getPositionX(void * p) { 
		return ((DeltaProtoDriverObj*)p)->getPositionX(); 
	}
	double getPositionY(void * p) { 
		return ((DeltaProtoDriverObj*)p)->getPositionY(); 
	}

#ifdef __cplusplus
        }
#endif 

int main(int argc, char* argv[])
{
}

DeltaProtoDriverObj::DeltaProtoDriverObj() 
{
}

void DeltaProtoDriverObj::homeAllNodes(IPort *myPort) {

	//IPort &myPort = myMgr->Ports(0);

	//iPort= &myPort;

	printf("homeAllNodes OpenState %i \n", myPort->OpenState());

	for (size_t iNode = 0; iNode < myPort->NodeCount(); iNode++) {
		try {
			// Create a shortcut reference for the first node
			INode &theNode = myPort->Nodes(iNode);

			theNode.EnableReq(false);				//Ensure Node is disabled before starting

			printf("   Node[0]: type=%d\n", theNode.Info.NodeType());
			printf("            userID: %s\n", theNode.Info.UserID.Value());
			printf("        FW version: %s\n", theNode.Info.FirmwareVersion.Value());
			printf("          Serial #: %d\n", theNode.Info.SerialNumber.Value());
			printf("             Model: %s\n", theNode.Info.Model.Value());

			m_sysManager->Delay(250);

			//The following statements will attempt to enable the node.  First,
			// any shutdowns or NodeStops are cleared, finally the node in enabled
			theNode.Status.AlertsClear();					//Clear Alerts on node 
			theNode.Motion.NodeStopClear();	//Clear Nodestops on Node  		

			m_sysManager->Delay(250);

			theNode.EnableReq(true);					//Enable node 

			double timeout = m_sysManager->TimeStampMsec() + TIME_TILL_TIMEOUT;	//define a timeout in case the node is unable to enable
																		//This will loop checking on the Real time values of the node's Ready status
			while (!theNode.Motion.IsReady()) {
				if (m_sysManager->TimeStampMsec() > timeout) {
					printf("Error: Timed out waiting for Node 0 to enable\n");
					return;
				}
			}
			//At this point the Node is enabled, and we will now check to see if the Node has been homed
			//Check the Node to see if it has already been homed, 
			if (theNode.Motion.Homing.WasHomed())
			{
				printf("Node has already been homed, current position is: \t%8.0f \n", theNode.Motion.PosnMeasured.Value());
				printf("Rehoming Node... \n");
			}
			else
			{
				printf("Node has not been homed.  Homing Node now...\n");
			}
			//Now we will home the Node

			m_sysManager->Delay(250);

			theNode.Motion.Homing.Initiate();

			timeout = m_sysManager->TimeStampMsec() + TIME_TILL_TIMEOUT;			//define a timeout longer than the longest time possible for the mechanics to reach the homing target
			//This will loop checking on the Real time values of the node's Homing active status
			while (theNode.Motion.Homing.IsHoming() && m_sysManager->TimeStampMsec() < timeout)
			{
				m_sysManager->Delay(1);
			}

			//Check to make sure we've homed
			if (theNode.Motion.Homing.WasHomed())
			{
				theNode.Motion.PosnMeasured.Refresh();		//Refresh our current measured position
				printf("Node completed homing, current position: \t%8.0f \n", theNode.Motion.PosnMeasured.Value());
				printf("Soft limits now active\n");
				
			}
			else
			{
				printf("Node did not complete homing:  \n\t -Ensure Homing settings have been defined through ClearView. \n\t -Check for alerts/Shutdowns \n\t -Ensure timeout is longer than the longest possible homing move.\n");
				//::system("pause");
					return;
			}
			//Printing soft limits
			printf("Soft Limit 1 = %d \n ", (int)theNode.Limits.SoftLimit1);
			printf("Soft Limit 2 = %d \n ", (int)theNode.Limits.SoftLimit2);

			//printf("Adjusting Numberspace by %d\n", CHANGE_NUMBER_SPACE);

			//theNode.Motion.AddToPosition(CHANGE_NUMBER_SPACE);			//Now the node is no longer considered "homed, and soft limits are turned off

			//theNode.Motion.Homing.SignalComplete();		//reset the Node's "sense of home" soft limits (unchanged) are now active again

			//theNode.Motion.PosnMeasured.Refresh();		//Refresh our current measured position
			//printf("Numberspace changed, current position: \t%8.0f \n", theNode.Motion.PosnMeasured.Value());
			//printf("Soft Limit 1 = %d \n ", (int)theNode.Limits.SoftLimit1);
			//printf("Soft Limit 2 = %d \n ", (int)theNode.Limits.SoftLimit2);

			//printf("Disabling Node");
			theNode.EnableReq(false);

			printf(" Port[%d]: state=%d, nodes=%d\n",
				myPort->NetNumber(), myPort->OpenState(), myPort->NodeCount());
		}
		catch (mnErr theErr)
		{
			//This statement will print the address of the error, the error code (defined by the mnErr class), 
			//as well as the corresponding error message.
			printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
		}
	}
}

void DeltaProtoDriverObj::openPort( int portNum ) {
	size_t portCount = 0;
	std::vector<std::string> comHubPorts;

	//Create the SysManager object. This object will coordinate actions among various ports
	// and within nodes. In this example we use this object to setup and open our port.
	SysManager* myMgr = SysManager::Instance();									//Create System Manager myMgr

	printf("openPort%i\n", portNum);

	m_portNum= portNum;

	//This will try to open the port. If there is an error/exception during the port opening,
	//the code will jump to the catch loop where detailed information regarding the error will be displayed;
	//otherwise the catch loop is skipped over
	try
	{ 
		SysManager::FindComHubPorts(comHubPorts);
		printf("Found %d SC Hubs\n", comHubPorts.size());

		for (portCount = 0; portCount < comHubPorts.size() && portCount < NET_CONTROLLER_MAX; portCount++) {
			
			myMgr->ComHubPort(portCount, comHubPorts[portCount].c_str()); 	//define the first SC Hub port (port 0) to be associated 
											// with COM portnum (as seen in device manager)
		}

		if (portCount > 0) {
			//printf("\n I will now open port \t%i \n \n", portnum);
			myMgr->PortsOpen(portCount);				//Open the port

			//IPort &myPort = myMgr->Ports(i);
			m_iPort = &(myMgr->Ports(0));

			for (size_t iNode = 0; iNode < m_iPort->NodeCount(); iNode++) {
				try {
					// Create a shortcut reference for the first node
					INode &theNode = m_iPort->Nodes(iNode);
					printf("   Node[0]: type=%d\n", theNode.Info.NodeType());
					printf("            userID: %s\n", theNode.Info.UserID.Value());
					printf("        FW version: %s\n", theNode.Info.FirmwareVersion.Value());
					printf("          Serial #: %d\n", theNode.Info.SerialNumber.Value());
					printf("             Model: %s\n", theNode.Info.Model.Value());

					if( iNode == 0 ) {
						m_node_x= &theNode;
					}
					else {
						m_node_y= &theNode;
					}
				}
				catch (mnErr theErr)
				{
					//This statement will print the address of the error, the error code (defined by the mnErr class), 
					//as well as the corresponding error message.
					printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
				}
			}
		}
		else {
			printf("Unable to locate SC hub port\n");

			//msgUser("Press any key to continue."); //pause so the user can see the error message; waits for user to press a key

			//return -1;  //This terminates the main program
		}
	}			
	catch(mnErr theErr)	//This catch statement will intercept any error from the Class library
	{
		printf("Port Failed to open, Check to ensure correct Port number and that ClearView is not using the Port\n");	
		//This statement will print the address of the error, the error code (defined by the mnErr class), 
		//as well as the corresponding error message.
		printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
	}

	m_sysManager= myMgr;

	printf("OpenState %i \n", m_iPort->OpenState());

	if( m_iPort->OpenState() != 5 ) {
		m_iPort->RestartCold();
		printf("RestartCold \n" );
		printf("OpenState %i \n", m_iPort->OpenState());
	}

	

    return;
}

void DeltaProtoDriverObj::home() {

	printf("home\n" );

	if( m_iPort->OpenState() != 5 ) {
		m_iPort->RestartCold();
		printf("RestartCold \n" );

		printf("OpenState %i \n", m_iPort->OpenState());

		printf("NodeCount %i \n", m_iPort->NodeCount());
	}

	homeAllNodes(m_iPort);

	for (size_t iNode = 0; iNode < m_iPort->NodeCount(); iNode++) {
		try {
			// Create a shortcut reference for the first node
			INode &theNode = m_iPort->Nodes(iNode);
			printf("   Node[0]: type=%d\n", theNode.Info.NodeType());
			printf("            userID: %s\n", theNode.Info.UserID.Value());
			printf("        FW version: %s\n", theNode.Info.FirmwareVersion.Value());
			printf("          Serial #: %d\n", theNode.Info.SerialNumber.Value());
			printf("             Model: %s\n", theNode.Info.Model.Value());

			//DeltaProto 3DP X axis

			if( iNode == 0 ) {
				m_node_x= &theNode;
			}
			else {
				m_node_y= &theNode;
			}
		}
		catch (mnErr theErr)
		{
			//This statement will print the address of the error, the error code (defined by the mnErr class), 
			//as well as the corresponding error message.
			printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
		}
	}
}

void DeltaProtoDriverObj::moveTo( int x, int y, int acc, int vel, int release ) {

    moveToDelay(x,y,acc,vel,release,0,0);
}

void DeltaProtoDriverObj::moveToDelay( int x, int y, int acc, int vel, int release, int delayX, int delayY ) {

	x= x * -1;
	y= y * -1;

	printf("moveTo x %d y %d dx %d dy %d\n", x, y, delayX, delayY);

	printf("OpenState %i \n", m_iPort->OpenState());

	printf("NodeCount %i \n", m_iPort->NodeCount());

	if( acc < ACC_LIM_RPM_PER_SEC_MIN || ACC_LIM_RPM_PER_SEC_MAX < acc ) {
		printf("Acceleration exceeds min/max value %d \n", acc);
		return;
	}

	if( vel < VEL_LIM_RPM_MIN || VEL_LIM_RPM_MAX < vel ) {
		printf("Velocity exceeds min/max value %d \n", vel);
		return;
	}

	if( m_iPort->OpenState() != 5 ) {
		m_iPort->RestartCold();
		printf("RestartCold \n" );

		printf("OpenState %i \n", m_iPort->OpenState());

		printf("NodeCount %i \n", m_iPort->NodeCount());
	}

	if (m_node_x->Motion.Homing.WasHomed() && m_node_y->Motion.Homing.WasHomed())
	{
		try {

			printf("X userID: %s\n", m_node_x->Info.UserID.Value());
			printf("Y userID: %s\n", m_node_y->Info.UserID.Value());

			m_node_x->Motion.MoveWentDone();						//Clear the rising edge Move done register

			m_node_x->AccUnit(INode::RPM_PER_SEC);				//Set the units for Acceleration to RPM/SEC
			m_node_x->VelUnit(INode::RPM);						//Set the units for Velocity to RPM
			m_node_x->Motion.AccLimit = acc;		//Set Acceleration Limit (RPM/Sec)
			m_node_x->Motion.VelLimit = vel;				//Set Velocity Limit (RPM)

			m_node_y->Motion.MoveWentDone();						//Clear the rising edge Move done register

			m_node_y->AccUnit(INode::RPM_PER_SEC);				//Set the units for Acceleration to RPM/SEC
			m_node_y->VelUnit(INode::RPM);						//Set the units for Velocity to RPM
			m_node_y->Motion.AccLimit = acc;		//Set Acceleration Limit (RPM/Sec)
			m_node_y->Motion.VelLimit = vel;				//Set Velocity Limit (RPM)

			m_node_x->EnableReq(true);		
			m_node_y->EnableReq(true);		

            if( delayX > delayY ) {

                printf("Moving Node y \t%i \n", m_node_y);
                m_node_y->Motion.MovePosnStart(y, true);

                m_sysManager->Delay(delayX);
                printf("Moving Node x \t%i \n", m_node_x);
                m_node_x->Motion.MovePosnStart(x, true);
			}
			else {
                printf("Moving Node x \t%i \n", m_node_x);
                m_node_x->Motion.MovePosnStart(x, true);

                m_sysManager->Delay(delayY);
                printf("Moving Node y \t%i \n", m_node_y);
                m_node_y->Motion.MovePosnStart(y, true);
			}
		}
		catch (mnErr theErr)
		{
			//This statement will print the address of the error, the error code (defined by the mnErr class), 
			//as well as the corresponding error message.
			printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
		}
	}
	else
	{
		printf("Node did not complete homing:  \n\t -Ensure Homing settings have been defined through ClearView. \n\t -Check for alerts/Shutdowns \n\t -Ensure timeout is longer than the longest possible homing move.\n");
		return;
	}
	
	for (size_t iNode = 0; iNode < m_iPort->NodeCount(); iNode++) {
		try {
			// Create a shortcut reference for the first node
			INode &theNode = m_iPort->Nodes(iNode);
			
			double timeout = m_sysManager->TimeStampMsec() + theNode.Motion.MovePosnDurationMsec(x) + 1000;			//define a timeout in case the node is unable to enable
																	
			while (!theNode.Motion.MoveWentDone()) {
				if (m_sysManager->TimeStampMsec() > timeout) {
					printf("Error: Timed out waiting for move to complete\n");
					return;
				}
			}
			printf("Node \t%i Move Done\n", iNode);

			if( release ) {
				theNode.EnableReq(false);	
			}	
		}
		catch (mnErr theErr)
		{
			//This statement will print the address of the error, the error code (defined by the mnErr class), 
			//as well as the corresponding error message.
			printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
		}
	}

}

void DeltaProtoDriverObj::moveToX( int x, int acc, int vel, int release ) {

	x= x * -1;

	printf("moveTo x %d\n", x);

	printf("OpenState %i \n", m_iPort->OpenState());

	printf("NodeCount %i \n", m_iPort->NodeCount());

	if( acc < ACC_LIM_RPM_PER_SEC_MIN || ACC_LIM_RPM_PER_SEC_MAX < acc ) {
		printf("Acceleration exceeds min/max value %d \n", acc);
		return;
	}

	if( vel < VEL_LIM_RPM_MIN || VEL_LIM_RPM_MAX < vel ) {
		printf("Velocity exceeds min/max value %d \n", vel);
		return;
	}

	if( m_iPort->OpenState() != 5 ) {
		m_iPort->RestartCold();
		printf("RestartCold \n" );

		printf("OpenState %i \n", m_iPort->OpenState());

		printf("NodeCount %i \n", m_iPort->NodeCount());
	}

	if (m_node_x->Motion.Homing.WasHomed() && m_node_y->Motion.Homing.WasHomed())
	{
		try {

			printf("X userID: %s\n", m_node_x->Info.UserID.Value());

			m_node_x->Motion.MoveWentDone();						//Clear the rising edge Move done register

			m_node_x->AccUnit(INode::RPM_PER_SEC);				//Set the units for Acceleration to RPM/SEC
			m_node_x->VelUnit(INode::RPM);						//Set the units for Velocity to RPM
			m_node_x->Motion.AccLimit = acc;		//Set Acceleration Limit (RPM/Sec)
			m_node_x->Motion.VelLimit = vel;				//Set Velocity Limit (RPM)

			m_node_x->EnableReq(true);		

			printf("Moving Node x \t%i \n", m_node_x);
			m_node_x->Motion.MovePosnStart(x, true);
		}
		catch (mnErr theErr)
		{
			//This statement will print the address of the error, the error code (defined by the mnErr class), 
			//as well as the corresponding error message.
			printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
		}
	}
	else
	{
		printf("Node did not complete homing:  \n\t -Ensure Homing settings have been defined through ClearView. \n\t -Check for alerts/Shutdowns \n\t -Ensure timeout is longer than the longest possible homing move.\n");
		return;
	}
	
	for (size_t iNode = 0; iNode < m_iPort->NodeCount(); iNode++) {
		try {
			
			double timeout = m_sysManager->TimeStampMsec() + m_node_x->Motion.MovePosnDurationMsec(x) + 1000;			//define a timeout in case the node is unable to enable
																	
			while (!m_node_x->Motion.MoveWentDone()) {
				if (m_sysManager->TimeStampMsec() > timeout) {
					printf("Error: Timed out waiting for move to complete\n");
					return;
				}
			}
			printf("Node \t%i Move Done\n", m_node_x);

			if( release ) {
				m_node_x->EnableReq(false);	
			}	
		}
		catch (mnErr theErr)
		{
			//This statement will print the address of the error, the error code (defined by the mnErr class), 
			//as well as the corresponding error message.
			printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
		}
	}

}

void DeltaProtoDriverObj::moveToY( int y, int acc, int vel, int release ) {

	y= y * -1;

	printf("moveTo y %d\n", y);

	printf("OpenState %i \n", m_iPort->OpenState());

	printf("NodeCount %i \n", m_iPort->NodeCount());

	if( acc < ACC_LIM_RPM_PER_SEC_MIN || ACC_LIM_RPM_PER_SEC_MAX < acc ) {
		printf("Acceleration exceeds min/max value %d \n", acc);
		return;
	}

	if( vel < VEL_LIM_RPM_MIN || VEL_LIM_RPM_MAX < vel ) {
		printf("Velocity exceeds min/max value %d \n", vel);
		return;
	}

	if( m_iPort->OpenState() != 5 ) {
		m_iPort->RestartCold();
		printf("RestartCold \n" );

		printf("OpenState %i \n", m_iPort->OpenState());

		printf("NodeCount %i \n", m_iPort->NodeCount());
	}

	if (m_node_x->Motion.Homing.WasHomed() && m_node_y->Motion.Homing.WasHomed())
	{
		try {

			printf("Y userID: %s\n", m_node_y->Info.UserID.Value());

			m_node_y->Motion.MoveWentDone();						//Clear the rising edge Move done register

			m_node_y->AccUnit(INode::RPM_PER_SEC);				//Set the units for Acceleration to RPM/SEC
			m_node_y->VelUnit(INode::RPM);						//Set the units for Velocity to RPM
			m_node_y->Motion.AccLimit = acc;		//Set Acceleration Limit (RPM/Sec)
			m_node_y->Motion.VelLimit = vel;				//Set Velocity Limit (RPM)

			m_node_y->EnableReq(true);		

			printf("Moving Node y \t%i \n", m_node_y);
			m_node_y->Motion.MovePosnStart(y, true);
		}
		catch (mnErr theErr)
		{
			//This statement will print the address of the error, the error code (defined by the mnErr class), 
			//as well as the corresponding error message.
			printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
		}
	}
	else
	{
		printf("Node did not complete homing:  \n\t -Ensure Homing settings have been defined through ClearView. \n\t -Check for alerts/Shutdowns \n\t -Ensure timeout is longer than the longest possible homing move.\n");
		return;
	}
	
	for (size_t iNode = 0; iNode < m_iPort->NodeCount(); iNode++) {
		try {
			
			double timeout = m_sysManager->TimeStampMsec() + m_node_y->Motion.MovePosnDurationMsec(y) + 1000;			//define a timeout in case the node is unable to enable
																	
			while (!m_node_y->Motion.MoveWentDone()) {
				if (m_sysManager->TimeStampMsec() > timeout) {
					printf("Error: Timed out waiting for move to complete\n");
					return;
				}
			}
			printf("Node \t%i Move Done\n", m_node_y);

			if( release ) {
				m_node_y->EnableReq(true);		
			}	
		}
		catch (mnErr theErr)
		{
			//This statement will print the address of the error, the error code (defined by the mnErr class), 
			//as well as the corresponding error message.
			printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
		}
	}

}

void DeltaProtoDriverObj::release( int rel ) {

	printf("releaseMotor %d\n", rel);

	printf("OpenState %i \n", m_iPort->OpenState());

	printf("NodeCount %i \n", m_iPort->NodeCount());

	if( m_iPort->OpenState() != 5 ) {
		m_iPort->RestartCold();
		printf("RestartCold \n" );

		printf("OpenState %i \n", m_iPort->OpenState());

		printf("NodeCount %i \n", m_iPort->NodeCount());
	}
	
	for (size_t iNode = 0; iNode < m_iPort->NodeCount(); iNode++) {
		try {
			// Create a shortcut reference for the first node
			INode &theNode = m_iPort->Nodes(iNode);
			
			/**double timeout = m_sysManager->TimeStampMsec() + 1000;			//define a timeout in case the node is unable to enable
																	
			while (!theNode.Motion.MoveWentDone()) {
				if (m_sysManager->TimeStampMsec() > timeout) {
					printf("Error: Timed out waiting for move to complete\n");
					return;
				}
			}
			printf("Node \t%i Move Done\n", iNode);**/

			if( rel ) {
				theNode.EnableReq(false);	
			}	
		}
		catch (mnErr theErr)
		{
			//This statement will print the address of the error, the error code (defined by the mnErr class), 
			//as well as the corresponding error message.
			printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
		}
	}
	
}

double DeltaProtoDriverObj::getPositionX() {
	//Check to make sure we've homed
	if (m_node_x->Motion.Homing.WasHomed())
	{
		m_node_x->Motion.PosnMeasured.Refresh();		//Refresh our current measured position

		double pos= m_node_x->Motion.PosnMeasured.Value();

		printf("Node completed homing, current position: \t%8.0f \n", pos);
		printf("Soft limits now active\n");
		
		return pos * -1;
	}
	else
	{
		printf("Node did not complete homing:  \n\t -Ensure Homing settings have been defined through ClearView. \n\t -Check for alerts/Shutdowns \n\t -Ensure timeout is longer than the longest possible homing move.\n");
		//::system("pause");
		return -1;
	}
}

double DeltaProtoDriverObj::getPositionY() {
	//Check to make sure we've homed
	if (m_node_y->Motion.Homing.WasHomed())
	{
		m_node_y->Motion.PosnMeasured.Refresh();		//Refresh our current measured position

		double pos= m_node_y->Motion.PosnMeasured.Value();

		printf("Node completed homing, current position: \t%8.0f \n", pos);
		printf("Soft limits now active\n");
		
		return pos * -1;
	}
	else
	{
		printf("Node did not complete homing:  \n\t -Ensure Homing settings have been defined through ClearView. \n\t -Check for alerts/Shutdowns \n\t -Ensure timeout is longer than the longest possible homing move.\n");
		//::system("pause");
		return -1;
	}
}
 
void DeltaProtoDriverObj::closePort() {

	printf("PortsClose");
	m_sysManager->PortsClose();
}

