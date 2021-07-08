/* rpccalls.h */
#ifndef __RPCCALL__
#define __RPCCALL__

#define MAX_BLOCK_SIZE        1024*1024


int  rpcopen( char *hostName );
int  rpcclose();

long  get_vme_a32_long( long addr);
short get_vme_a32_short(long addr);
char  get_vme_a32_char( long addr);

long  get_vme_a24_long( long addr);
short get_vme_a24_short(long addr);
char  get_vme_a24_char( long addr);

long  get_vme_a16_long( long addr);
short get_vme_a16_short(long addr);
char  get_vme_a16_char( long addr);

int  get_vme_a32_block( long addr, long buff[], long size);


int set_vme_a32_long( long addr, long  data );
int set_vme_a32_short(long addr, short data );
int set_vme_a32_char( long addr, char  data );

int set_vme_a24_long( long addr, long  data );
int set_vme_a24_short(long addr, short data );
int set_vme_a24_char( long addr, char  data );

int set_vme_a16_long( long addr, long  data );
int set_vme_a16_short(long addr, short data );
int set_vme_a16_char( long addr, char  data );

/* read write format of the above */
int  ReadVMEA32Long( long addr, long  *data);
int  ReadVMEA32Short(long addr, short *data);
int  ReadVMEA32Char( long addr, char  *data);

int  ReadVMEA24Long( long addr, long  *data);
int  ReadVMEA24Short(long addr, short *data);
int  ReadVMEA24Char( long addr, char  *data);

int  ReadVMEA16Long( long addr, long  *data);
int  ReadVMEA16Short(long addr, short *data);
int  ReadVMEA16Char( long addr, char  *data);

int  ReadVMEA32Block( long addr, long *buff, long size);


int  WriteVMEA32Long( long addr, long  data );
int  WriteVMEA32Short(long addr, short data );
int  WriteVMEA32Char( long addr, char  data );

int  WriteVMEA24Long( long addr, long  data );
int  WriteVMEA24Short(long addr, short data );
int  WriteVMEA24Char( long addr, char  data );

int  WriteVMEA16Long( long addr, long  data );
int  WriteVMEA16Short(long addr, short data );
int  WriteVMEA16Char( long addr, char  data );

#endif