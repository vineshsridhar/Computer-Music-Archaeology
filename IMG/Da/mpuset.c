int MpuCmdCnt=0;
#define MaxMpuBuf 1024
unsigned char MpuCmdBuf[MaxMpuBuf];

MpuSet(c)
	unsigned char c;
/*
** Append 'c' to 'MpuCmdBuf' (a global buffer of midi commands).
** At any time, the buffer contains 'MpuCmdCnt' commands.
*/
{
	if (MpuCmdCnt < MaxMpuBuf) MpuCmdBuf[MpuCmdCnt++] = c;
	return MpuCmdCnt;
}

ZeroMpuCmdCnt()
/*
** Zero the 'MpuCmdBuf'.  Done after an 'MpuFlush()'.
*/
{
	MpuCmdCnt = 0;
}

MpuFlush(f)
/*
** Write the buffered commands in 'MpuCmdBuf' to 'f'.
** Return '1' if something went wrong, '0' otherwise.
*/
{
	int i = MpuCmdCnt;
	MpuCmdCnt = 0;
	if (i==0) return 0;
	return write(f,MpuCmdBuf,i) != i;
}
