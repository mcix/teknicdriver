#include "DeltaProtoDriverImpl.h"
#include <jni.h>

#include <stdio.h>		
#include "pubSysCls.h"	
#include <thread>

using namespace sFnd;

#define ACC_LIM_RPM_PER_SEC	200
#define VEL_LIM_RPM			500
#define TIME_TILL_TIMEOUT	20000	//The timeout used for homing(ms)

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
	void closePort();
	void moveTo( int x, int y);
	void setVar( int x );
	int getVar();
private:
	void homeAllNodes(SysManager *myMgr);
};


#ifdef __cplusplus
        extern "C" {
#endif

	void * 	createDriver(int portNum) { 
		DeltaProtoDriverObj* dpd= new DeltaProtoDriverObj(); 
		dpd->openPort(portNum);
		return dpd;
	}
	void    freeDriver(void * p) { 
		((DeltaProtoDriverObj*)p)->closePort();
		delete p; 
	}
	void moveTo(void * p, int x, int y) { 
		((DeltaProtoDriverObj*)p)->moveTo(x, y); 
	}
	void setVar(void * p, int x) { 
		((DeltaProtoDriverObj*)p)->setVar(x); 
	}
	int getVar(void * p) { 
		return ((DeltaProtoDriverObj*)p)->getVar(); 
	}

#ifdef __cplusplus
        }
#endif 

DeltaProtoDriverObj::DeltaProtoDriverObj() 
{
}

void DeltaProtoDriverObj::setVar( int x ) {
	m_var= x;
}

int DeltaProtoDriverObj::getVar() {
	return m_var;
}

void DeltaProtoDriverObj::homeAllNodes(SysManager *myMgr) {

	IPort &myPort = myMgr->Ports(0);

	//iPort= &myPort;

	for (size_t iNode = 0; iNode < myPort.NodeCount(); iNode++) {
		try {
			// Create a shortcut reference for the first node
			INode &theNode = myPort.Nodes(iNode);

			theNode.EnableReq(false);				//Ensure Node is disabled before starting

			printf("   Node[0]: type=%d\n", theNode.Info.NodeType());
			printf("            userID: %s\n", theNode.Info.UserID.Value());
			printf("        FW version: %s\n", theNode.Info.FirmwareVersion.Value());
			printf("          Serial #: %d\n", theNode.Info.SerialNumber.Value());
			printf("             Model: %s\n", theNode.Info.Model.Value());

			//The following statements will attempt to enable the node.  First,
			// any shutdowns or NodeStops are cleared, finally the node in enabled
			theNode.Status.AlertsClear();					//Clear Alerts on node 
			theNode.Motion.NodeStopClear();	//Clear Nodestops on Node  				
			theNode.EnableReq(true);					//Enable node 

			double timeout = myMgr->TimeStampMsec() + TIME_TILL_TIMEOUT;	//define a timeout in case the node is unable to enable
																		//This will loop checking on the Real time values of the node's Ready status
			while (!theNode.Motion.IsReady()) {
				if (myMgr->TimeStampMsec() > timeout) {
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

			theNode.Motion.Homing.Initiate();

			timeout = myMgr->TimeStampMsec() + TIME_TILL_TIMEOUT;			//define a timeout longer than the longest time possible for the mechanics to reach the homing target
			//This will loop checking on the Real time values of the node's Homing active status
			while (theNode.Motion.Homing.IsHoming() && myMgr->TimeStampMsec() < timeout)
			{
				myMgr->Delay(1);
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
			//theNode.EnableReq(false);

			printf(" Port[%d]: state=%d, nodes=%d\n",
				myPort.NetNumber(), myPort.OpenState(), myPort.NodeCount());
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
	//Create the SysManager object. This object will coordinate actions among various ports
	// and within nodes. In this example we use this object to setup and open our port.
	SysManager myMgr;							//Create System Manager myMgr
	
	printf("Hello World, I am SysManager\n");	//Print to console

	printf("\n I will now open port \t%i \n \n", portNum);

	m_portNum= portNum;

	//This will try to open the port. If there is an error/exception during the port opening,
	//the code will jump to the catch loop where detailed information regarding the error will be displayed;
	//otherwise the catch loop is skipped over
	try
	{ 
		myMgr.ComHubPort(0, portNum); 	//define the first SC Hub port (port 0) to be associated 
										// with COM portNum (as seen in device manager)
										
		myMgr.PortsOpen(1);				//Open the port
	}			
	catch(mnErr theErr)	//This catch statement will intercept any error from the Class library
	{
		printf("Port Failed to open, Check to ensure correct Port number and that ClearView is not using the Port\n");	
		//This statement will print the address of the error, the error code (defined by the mnErr class), 
		//as well as the corresponding error message.
		printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
	}

	homeAllNodes(&myMgr);

	m_sysManager= &myMgr;

	m_iPort = &(myMgr.Ports(0));
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

	printf("OpenState %i \n", m_iPort->OpenState());

    return;
}

void DeltaProtoDriverObj::moveTo( int x, int y ) {

	printf("Move x %d y %d\n", x, y);

	printf("OpenState %i \n", m_iPort->OpenState());

	printf("NodeCount %i \n", m_iPort->NodeCount());

	if( m_iPort->OpenState() != 5 ) {
		m_iPort->RestartCold();
		printf("RestartCold \n" );

		printf("OpenState %i \n", m_iPort->OpenState());

		printf("NodeCount %i \n", m_iPort->NodeCount());
	}
	
	//printf("WaitForOnline %i \n", m_iPort->WaitForOnline());

	//This will try to open the port. If there is an error/exception during the port opening,
	//the code will jump to the catch loop where detailed information regarding the error will be displayed;
	//otherwise the catch loop is skipped over
	try
	{ 
		//m_sysManager->ComHubPort(0, m_portNum); 	//define the first SC Hub port (port 0) to be associated 
										// with COM portNum (as seen in device manager)
										
		//m_sysManager->PortsOpen(1);				//Open the port
	}			
	catch(mnErr theErr)	//This catch statement will intercept any error from the Class library
	{
		printf("Port Failed to open, Check to ensure correct Port number and that ClearView is not using the Port\n");	
		//This statement will print the address of the error, the error code (defined by the mnErr class), 
		//as well as the corresponding error message.
		printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
	}

	//this doesn't return anything:
	/*IPort &iPort = m_sysManager->Ports(0);
	for (size_t iNode = 0; iNode < iPort.NodeCount(); iNode++) {
		try {
			// Create a shortcut reference for the first node
			INode &theNode = iPort.Nodes(iNode);
			printf("   Node[0]: type=%d\n", theNode.Info.NodeType());
			printf("            userID: %s\n", theNode.Info.UserID.Value());
			printf("        FW version: %s\n", theNode.Info.FirmwareVersion.Value());
			printf("          Serial #: %d\n", theNode.Info.SerialNumber.Value());
			printf("             Model: %s\n", theNode.Info.Model.Value());
		}
		catch (mnErr theErr)
		{
			//This statement will print the address of the error, the error code (defined by the mnErr class), 
			//as well as the corresponding error message.
			printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
		}
	}*/

	//this also doesn't return anything:
	for (size_t iNode = 0; iNode < m_iPort->NodeCount(); iNode++) {
		try {
			// Create a shortcut reference for the first node
			INode &theNode = m_iPort->Nodes(iNode);
			printf("   Node[0]: type=%d\n", theNode.Info.NodeType());
			printf("            userID: %s\n", theNode.Info.UserID.Value());
			printf("        FW version: %s\n", theNode.Info.FirmwareVersion.Value());
			printf("          Serial #: %d\n", theNode.Info.SerialNumber.Value());
			printf("             Model: %s\n", theNode.Info.Model.Value());

			theNode.Motion.MoveWentDone();						//Clear the rising edge Move done register

			theNode.AccUnit(INode::RPM_PER_SEC);				//Set the units for Acceleration to RPM/SEC
			theNode.VelUnit(INode::RPM);						//Set the units for Velocity to RPM
			theNode.Motion.AccLimit = ACC_LIM_RPM_PER_SEC;		//Set Acceleration Limit (RPM/Sec)
			theNode.Motion.VelLimit = VEL_LIM_RPM;				//Set Velocity Limit (RPM)

			theNode.EnableReq(true);		

			printf("Moving Node \t%i \n", iNode);
			theNode.Motion.MovePosnStart(x);			//Execute 10000 encoder count move 

			double timeout = m_sysManager->TimeStampMsec() + theNode.Motion.MovePosnDurationMsec(x) + 1000;			//define a timeout in case the node is unable to enable
																	

			while (!theNode.Motion.MoveWentDone()) {
				if (m_sysManager->TimeStampMsec() > timeout) {
					printf("Error: Timed out waiting for move to complete\n");
					return;
				}
			}
			printf("Node \t%i Move Done\n", iNode);

			theNode.EnableReq(false);		
		}
		catch (mnErr theErr)
		{
			//This statement will print the address of the error, the error code (defined by the mnErr class), 
			//as well as the corresponding error message.
			printf("Caught error: addr=%d, err=0x%08x\nmsg=%s\n", theErr.TheAddr, theErr.ErrorCode, theErr.ErrorMsg);
		}
	}

	//this will throw a stacktrace:
	//printf("  userID: %s\n", m_node_x->Info.UserID.Value());
	//printf("  userID: %s\n", m_node_y->Info.UserID.Value());
	
}
 
void DeltaProtoDriverObj::closePort() {

	printf("PortsClose");
	m_sysManager->PortsClose();
}

