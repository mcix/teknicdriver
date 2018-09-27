#ifndef _DELTA_PROTO_DRIVER_IMPL_H
#define _DELTA_PROTO_DRIVER_IMPL_H

#define ACC_LIM_RPM_PER_SEC_MIN	100
#define VEL_LIM_RPM_MIN			50
#define ACC_LIM_RPM_PER_SEC_MAX	1000
#define VEL_LIM_RPM_MAX			1600

#ifdef __cplusplus
        extern "C" {
#endif
       
        void * 	createDriver(int portNum);
        void    home(void * p);
    	void    freeDriver(void * p);
    	void  	moveTo(void * p, int x, int y, int acc, int vel, int release);
    	void  	moveToDelay(void * p, int x, int y, int acc, int vel, int release, int delayX, int delayY);
    	void  	moveToX(void * p, int x, int acc, int vel, int release);
    	void  	moveToY(void * p, int y, int acc, int vel, int release);
    	void  	release(void * p, int release);
    	double 	getPositionX(void * p);
    	double 	getPositionY(void * p);

#ifdef __cplusplus
        }
#endif
 
#endif