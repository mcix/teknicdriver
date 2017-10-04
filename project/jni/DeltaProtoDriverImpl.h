#ifndef _DELTA_PROTO_DRIVER_IMPL_H
#define _DELTA_PROTO_DRIVER_IMPL_H

#ifdef __cplusplus
        extern "C" {
#endif
       
        void * 	createDriver(int portNum);
    	void    freeDriver(void * p);
    	void  	moveTo(void * p, int x, int y);

    	void  	setVar(void * p, int x );
    	int  	getVar(void * p);

#ifdef __cplusplus
        }
#endif
 
#endif