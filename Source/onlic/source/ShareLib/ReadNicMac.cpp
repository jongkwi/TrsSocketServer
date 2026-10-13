
#include "ShareLib.h"

#ifdef _WIN32
#include <IPHlpApi.h>                       // for GetAdaptersInfo()
#pragma comment(lib, "iphlpapi.lib")

int ReadNicMac(int nPos, char *szNicMac, int log_view)
{
	int ret = TRUE;
	int i, reMem = FALSE;
	DWORD dwStatus = 0;
    ULONG ulSizeAdapterInfo = 0;
    IP_ADAPTER_INFO *pAdapterInfo = NULL, *pNewAdapterInfo = NULL;

	dwStatus = GetAdaptersInfo(pNewAdapterInfo, &ulSizeAdapterInfo);
    if (dwStatus == ERROR_BUFFER_OVERFLOW)    // GetAdaptersInfo가 메모리가 부족하면 재 할당하고 재호출
    {
        pNewAdapterInfo = (PIP_ADAPTER_INFO)Malloc(ulSizeAdapterInfo);
        dwStatus = GetAdaptersInfo(pNewAdapterInfo, &ulSizeAdapterInfo);
    }
	if(dwStatus == ERROR_SUCCESS) pAdapterInfo = pNewAdapterInfo;

	//Nic Card Proc
	for(i = 0; i < nPos && pAdapterInfo != NULL; i++) pAdapterInfo = pAdapterInfo->Next;

	if(pAdapterInfo == NULL) return FALSE;
	if(pAdapterInfo->Type == IF_TYPE_IEEE80211 || pAdapterInfo->Type == IF_TYPE_ETHERNET_CSMACD || pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET){
		sprintf_st(szNicMac, "%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
			pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2],
			pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
	}
	else{
		strcpy_st(szNicMac, "");
	}

	if(pNewAdapterInfo != NULL){
		free(pNewAdapterInfo);
		pNewAdapterInfo = NULL;
	}
	return ret;
}
#endif


#if defined(_FREEBSD)

void RunPingThread(void *vHandle)
{
	char szCmd[51], szIP[51];
	strcpy_st(szIP, (const char*)vHandle);

#ifdef _WIN32
	execl("ping", "ping", "211.187.141.211", NULL);
#elif _FREEBSD
	sprintf_st(szCmd, "ping -t 3 %s", "192.168.1.2");
	printf(szCmd);
	system(szCmd);
#elif _LINUX
	execl("/sbin/ping", "ping", "-W", "1", "211.187.141.211", NULL);
#elif _SOLARIS
	execl("/sbin/ping", "ping", "211.187.141.211", NULL);
#else
	execl("/sbin/ping", "ping", "211.187.141.211", NULL);
#endif
}

const char * Sock_Ntop(int family, const void *addrptr, char *strptr, size_t len)
{
	const u_char *p = (const u_char *) addrptr;
	char temp[INET_ADDRSTRLEN];

	
	if (family == AF_INET) {
		sprintf_st(temp, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
		if (strlen_st(temp) == len) {
			errno = ENOSPC;
			return (NULL);
		}
		strcpy_st(strptr, temp);
		return (strptr);
	}

	errno = EAFNOSUPPORT;

	return (NULL);
}

int DeviceSetOption(int devH, char *szNic)
{
	struct bpf_version bv;
	struct ifreq ifr;
	int nIMMFlag =1;			
	unsigned int uSize = 0;
	
	bzero(&ifr, sizeof(ifr));
	strcpy_st((char *)ifr.ifr_name, (const char*)szNic);

	if(ioctl(devH, BIOCVERSION, (char *)&bv) < 0) return -1;
	if((bv.bv_major != BPF_MAJOR_VERSION)||(bv.bv_minor < BPF_MINOR_VERSION)) return -2;
	if(ioctl(devH, BIOCSETIF, &ifr) < 0) return -3;
	if(ioctl(devH, BIOCPROMISC, 0) < 0) return -4;
	if(ioctl(devH, BIOCGBLEN, (char*)&uSize) < 0) -5;
	if(ioctl(devH, BIOCIMMEDIATE, &nIMMFlag) < 0) -6;

	return 0;
}

void RecvMacAddr(int devH, char *szIp, char *szMac, int log_view)
{
	BPF_HEADER bpfhdr;
	ETH_HEADER ethhdr;
	IP_HEADER iphdr;
	ARP_HEADER arphdr;
	struct timeval	tval;
	fd_set		rset, rset1;
	int maxfd, nready, nRecLen, nHeaderSize, nIPHeadLen, nLoop = 0;
	short h_proto;
	unsigned char recBuf[4096+1];
	char szTmpIP1[80], szTmpIP2[80];
	pthread_t pid = 0;
	FD_ZERO(&rset);
	FD_SET(devH, &rset);	

//	pthread_create(&pid, 0, (void* (*)(void*))RunPingThread, &szIp);
//	sleep(1);

	for(;;){
		maxfd = devH + 1;
		memcpy(&rset1, &rset, sizeof(fd_set));
		memset_st(&recBuf, 0, 4096);
		tval.tv_sec = 1;
		tval.tv_usec = 0;

		errno = 0;
		if ( (nready = select(maxfd, &rset1, NULL, NULL, &tval)) < 0){
			if(nLoop++ > 10) break;
			continue;
		}

		if (FD_ISSET(devH, &rset)) {
			nRecLen = read(devH, &recBuf, 4096);

			if(nRecLen > 0){
				memcpy(&bpfhdr, &recBuf, sizeof(BPF_HEADER));			
				nHeaderSize = bpfhdr.bh_hdrlen;
				memcpy(&ethhdr, &recBuf[nHeaderSize], sizeof(ETH_HEADER));
				nHeaderSize += sizeof(ETH_HEADER);

				h_proto = ntohs(ethhdr.h_proto);
				
				if(log_view == TRUE){
					printf("ReadNicMac devH:%d, h_proto:%d, nRecLen:%d\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
						devH, h_proto, nRecLen,
						recBuf[0], recBuf[1], recBuf[2], recBuf[3], recBuf[4], recBuf[5], recBuf[6], recBuf[7], recBuf[8], recBuf[9],
						recBuf[10], recBuf[11], recBuf[12], recBuf[13], recBuf[14], recBuf[15], recBuf[16], recBuf[17], recBuf[18], recBuf[19],
						recBuf[20], recBuf[21], recBuf[22], recBuf[23], recBuf[24], recBuf[25], recBuf[26], recBuf[27], recBuf[28], recBuf[29],
						recBuf[30], recBuf[31], recBuf[32], recBuf[33], recBuf[34], recBuf[35], recBuf[36], recBuf[37], recBuf[38], recBuf[39],
						recBuf[40], recBuf[41], recBuf[42], recBuf[43], recBuf[44], recBuf[45], recBuf[46], recBuf[47], recBuf[48], recBuf[49],
						recBuf[50], recBuf[51], recBuf[52], recBuf[53], recBuf[54], recBuf[55], recBuf[56], recBuf[57], recBuf[58], recBuf[59],
						recBuf[60], recBuf[61], recBuf[62], recBuf[63], recBuf[64], recBuf[65], recBuf[66], recBuf[67], recBuf[68], recBuf[69],
						recBuf[70], recBuf[71], recBuf[72], recBuf[73], recBuf[74], recBuf[75], recBuf[76], recBuf[77], recBuf[78], recBuf[79],
						recBuf[80], recBuf[81], recBuf[82], recBuf[83], recBuf[84], recBuf[85], recBuf[86], recBuf[87], recBuf[88], recBuf[89],
						recBuf[90], recBuf[91], recBuf[92], recBuf[93], recBuf[94], recBuf[95], recBuf[96], recBuf[97], recBuf[98], recBuf[99]);
				}
				if(h_proto == 0x0806){
					memcpy(&arphdr, &recBuf[nHeaderSize], sizeof(ARP_HEADER));
					nHeaderSize += sizeof(ARP_HEADER);

					sprintf_st(szTmpIP1, "%d.%d.%d.%d",
						(int)arphdr.srcIPAddr[0], (int)arphdr.srcIPAddr[1],
						(int)arphdr.srcIPAddr[2], (int)arphdr.srcIPAddr[3]);
					sprintf_st(szTmpIP2, "%d.%d.%d.%d",
						(int)arphdr.srcIPAddr[0], (int)arphdr.srcIPAddr[1],
						(int)arphdr.srcIPAddr[2], (int)arphdr.srcIPAddr[3]);
					if(strcmp(szIp, (const char*)szTmpIP1) == 0){
						sprintf_st(szMac, "%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
							arphdr.srcEthAddr[0], arphdr.srcEthAddr[1], arphdr.srcEthAddr[2],
							arphdr.srcEthAddr[3], arphdr.srcEthAddr[4], arphdr.srcEthAddr[5]);
						break;
					}
					else if(strcmp(szIp, (const char*)szTmpIP2) == 0){
						sprintf_st(szMac, "%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
							arphdr.dstEthAddr[0], arphdr.dstEthAddr[1], arphdr.dstEthAddr[2],
							arphdr.dstEthAddr[3], arphdr.dstEthAddr[4], arphdr.dstEthAddr[5]);
						break;
					}
				}
				else if(h_proto == 0x0800){
					memcpy(&iphdr, &recBuf[nHeaderSize], sizeof(IP_HEADER));
					nIPHeadLen = iphdr.header_len * 4;
					nHeaderSize += nIPHeadLen;

					Sock_Ntop(AF_INET, (sockaddr*)&iphdr.saddr, (char*)szTmpIP1, 80);
					Sock_Ntop(AF_INET, (sockaddr*)&iphdr.daddr, (char*)szTmpIP2, 80);
					if(strcmp(szIp, (const char*)szTmpIP1) == 0){
						sprintf_st(szMac, "%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
							ethhdr.h_source[0], ethhdr.h_source[1], ethhdr.h_source[2], ethhdr.h_source[3], ethhdr.h_source[4], ethhdr.h_source[5]);
						break;
					}
					else if(strcmp(szIp, (const char*)szTmpIP2) == 0){
						sprintf_st(szMac, "%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
							ethhdr.h_dest[0], ethhdr.h_dest[1], ethhdr.h_dest[2], ethhdr.h_dest[3], ethhdr.h_dest[4], ethhdr.h_dest[5]);
						break;
					}
				}
			}
		}
	}
}

void GetMacAddr(char *szNic, char *szIp, char *szMac, int log_view)
{
	
	int i, nRet, devH, nOpenedCnt;
	char szDevice[21];
	
	strcpy_st(szMac, "");
	
	for(i = 0; i < 10; i++){
		sprintf_st(szDevice, "%s%d", NIC_DEV, i);	

		errno = 0;
		devH = open(szDevice, O_RDONLY);	
		if(devH <= 0) continue;

		nRet = DeviceSetOption(devH, szNic);
		if(nRet < 0) return;
		else break;
	}

	if(strncmp(szIp, "127.0.0.1", 9) == FALSE){
		strcpy_st(szMac, szIp);
	}
	else{
		RecvMacAddr(devH, szIp, szMac, log_view);
	}

	close(devH);
	devH = 0;
}

int interface_status(int sockfd, char *szNic)
{
	struct ifreq ifr;
	struct ifmediareq ifmr;

	/* get interface flags */
	memset_st(&ifr, 0, sizeof(ifr));
	strlcpy(ifr.ifr_name, szNic, sizeof(ifr.ifr_name));
	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
		printf("ioctl(SIOCGIFFLAGS) on dev:%s\n", szNic);
		goto inactive;
	}

	/*
	 * if one of UP and RUNNING flags is dropped,
	 * the interface is not active.
	 */
	if ((ifr.ifr_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING))
		goto inactive;


	memset_st(&ifmr, 0, sizeof(ifmr));
	strlcpy(ifmr.ifm_name, szNic, sizeof(ifmr.ifm_name));
	if (ioctl(sockfd, SIOCGIFMEDIA, (caddr_t)&ifmr) < 0) {
		if (errno != EINVAL) {
			printf("ioctl(SIOCGIFMEDIA) on %s: %m", szNic);
			goto active;
		}
		/*
		 * EINVAL (or ENOTTY) simply means that the interface
		 * does not support the SIOCGIFMEDIA ioctl. We regard it alive.
		 */
		goto active;
	}
	if (ifmr.ifm_status & IFM_AVALID) {
		switch (ifmr.ifm_active & IFM_NMASK) {
		case IFM_ETHER:
		case IFM_IEEE80211:
			if (ifmr.ifm_status & IFM_ACTIVE)
				goto active;
			else
				goto inactive;
			break;
		default:
			goto inactive;
		}
	}
inactive:
	return (0);
active:
	return (1);
}

int ReadNicMac(int nPos, char *szNicMac, int log_view)
{
    unsigned char      *u;
    int                sockfd, size  = 100;
    struct ifreq       *ifr, *ifr_old = NULL, s_ifr;
    struct ifconf      ifc;
    struct sockaddr_in sa;
    int i, j = 0, inc;
    char szNic[51], szIp[51];

	strcpy_st(szNicMac, "");
	strcpy_st(szNic, "");
	strcpy_st(szIp, "");

    if (0 > (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP))) {
        fprintf(stderr, "Cannot open socket.\n");
		return -1;
    }

    ifc.ifc_len = IFRSIZE;
    ifc.ifc_req = NULL;

    /* realloc buffer size until no overflow occurs  */
    if (NULL == (ifc.ifc_req = (ifreq*)realloc(ifc.ifc_req, IFRSIZE))) {
        fprintf(stderr, "Out of memory.\n");
		close(sockfd);
		return -2;
    }

    ifc.ifc_len = IFRSIZE;
    if (ioctl(sockfd, SIOCGIFCONF, &ifc)) {
        perror("ioctl SIOCFIFCONF");
		close(sockfd);
		return -3;
    }

    ifr = ifc.ifc_req;
    i = ifc.ifc_len;

    while (i > 0) {
        inc = ifr->ifr_addr.sa_len;

        if (inc < sizeof(ifr->ifr_addr))
            inc = sizeof(ifr->ifr_addr);
        inc += IFNAMSIZ;
        if (ifr_old && !strcmp(ifr->ifr_name, ifr_old->ifr_name)) {
            goto next_loop;  /* duplicate, skip it */
        }

        if (ioctl(sockfd, SIOCGIFFLAGS, ifr)) {
            goto next_loop;  /* failed to get flags, skip it */
        }

        if (0 == ioctl(sockfd, SIOCGIFADDR, ifr)) {
			if(j == nPos){
				strcpy_st(szNic, ifr->ifr_name);
				strcpy_st(szIp, inet_ntoa(inaddrr(ifr_addr.sa_data)));
				break;
			}
			else j++;
        }

next_loop:
        ifr_old = ifr;
        ifr = (struct ifreq*) (((char*) ifr) + inc);
        i -= inc;
    }

	if(log_view == TRUE) printf("ReadNicMac szNic:%s, szIp:%s\n", szNic, szIp);
	if(strlen_st(szNic) <= 0){
		close(sockfd);
		return -3;
	}

	if(interface_status(sockfd, szNic) <= 0){
		strcpy_st(szNicMac, szIp);
		return -4;
	}
	close(sockfd);
	GetMacAddr(szNic, szIp, szNicMac, log_view);
	return 1;
}

#endif


#if (defined(_HPUX) || defined(_AIX) || defined(_SOLARIS))


#undef DLPI_DEV

#ifdef _HPUX
static char *dlpi_dev[] = {"/dev/dlpi", ""};
#define DLPI_DEV
#endif

#ifdef _AIX
static char *dlpi_dev[] = {"/dev/dlpi/et", "/dev/dlpi/en", "/dev/dlpi/tr", "/dev/dlpi/fddi", ""};
#define DLPI_DEV
/* AIX: remember to set up /etc/pse.conf or /etc/dlpi.conf */
#endif

#ifdef _SOLARIS
static char *dlpi_dev[] = {"/dev/bge", "/dev/eri", "/dev/hme", "/dev/ie", "/dev/le", "/dev/bnx", ""};
#define DLPI_DEV
#endif

#ifndef DLPI_DEV
static char *dlpi_dev[] = {"/dev/dlpi", ""};
/* unknown OS - hope that this will work ??? */
#define DLPI_DEV
#endif

/***********************************************************************/
/* 
 * implementation 
 */

#define INSAP 22 
#define OUTSAP 24 


#define bcopy(source, destination, length) memcpy(destination, source, length)

#define AREA_SZ 5000 /*­=­* buffer length in bytes *­=­*/ 
static u_long ctl_area[AREA_SZ];
static u_long dat_area[AREA_SZ];
static struct strbuf ctl = {AREA_SZ, 0, (char *)ctl_area};
static struct strbuf dat = {AREA_SZ, 0, (char *)dat_area};
#define GOT_CTRL 1 
#define GOT_DATA 2 
#define GOT_BOTH 3 
#define GOT_INTR 4 
#define GOT_ERR 128 

/*­=­* get a message from a stream; return type of message *­=­*/
static int get_msg(int fd)
{
    int flags = 0;
    int res, ret;
    ctl_area[0] = 0;
    dat_area[0] = 0;
    ret = 0;
    res = getmsg(fd, &ctl, &dat, &flags);
    if(res < 0) {
        if(errno == EINTR) {
            return(GOT_INTR);
        } else {
            return(GOT_ERR);
        }
    }
    if(ctl.len > 0) {
        ret |= GOT_CTRL;
    }
    if(dat.len > 0) {
        ret |= GOT_DATA;
    }
    return(ret);
}

/*­=­* verify that dl_primitive in ctl_area = prim *­=­*/
static int check_ctrl(int prim)
{
    dl_error_ack_t *err_ack = (dl_error_ack_t *)ctl_area;
    if(err_ack->dl_primitive != prim) {
        return GOT_ERR;
    }
    return 0;
}

/*­=­* put a control message on a stream *­=­*/
static int put_ctrl(int fd, int len, int pri)
{
    ctl.len = len;
    if(putmsg(fd, &ctl, 0, pri) < 0) {
        return GOT_ERR;
    }
    return  0;
}

/*­=­* put a control + data message on a stream *­=­*/
static int put_both(int fd, int clen, int dlen, int pri)
{
    ctl.len = clen;
    dat.len = dlen;
    if(putmsg(fd, &ctl, &dat, pri) < 0) {
        return GOT_ERR;
    }
    return  0;
}

/*­=­* open file descriptor and attach *­=­*/
static int dl_open(const char *dev, int ppa, int *fd)
{
    dl_attach_req_t *attach_req = (dl_attach_req_t *)ctl_area;
    if((*fd = open(dev, O_RDWR)) == -1) {
        return GOT_ERR;
    }
    attach_req->dl_primitive = DL_ATTACH_REQ;
    attach_req->dl_ppa = ppa;
    put_ctrl(*fd, sizeof(dl_attach_req_t), 0);
    get_msg(*fd);
    return check_ctrl(DL_OK_ACK);
}

/*­=­* send DL_BIND_REQ *­=­*/
static int dl_bind(int fd, int sap, u_char *addr)
{
    dl_bind_req_t *bind_req = (dl_bind_req_t *)ctl_area;
    dl_bind_ack_t *bind_ack = (dl_bind_ack_t *)ctl_area;
    bind_req->dl_primitive = DL_BIND_REQ;
    bind_req->dl_sap = sap;
    bind_req->dl_max_conind = 1;
    bind_req->dl_service_mode = DL_CLDLS;
    bind_req->dl_conn_mgmt = 0;
    bind_req->dl_xidtest_flg = 0;
    put_ctrl(fd, sizeof(dl_bind_req_t), 0);
    get_msg(fd);
    if (GOT_ERR == check_ctrl(DL_BIND_ACK)) {
        return GOT_ERR;
    }
    bcopy((u_char *)bind_ack + bind_ack->dl_addr_offset, addr, bind_ack->dl_addr_length);
    return 0;
}

/***********************************************************************/
/*
 * interface:
 * function mac_addr_dlpi - get the mac address of the "first" interface
 *
 * parameter: addr: an array of six bytes, has to be allocated by the caller
 *
 * return: 0 if OK, -1 if the address could not be determined
 *
 */

long mac_addr_dlpi (char *dev, u_char  *addr, int ppa)
{
	int fd;
	u_char mac_addr[25];

	if (GOT_ERR != dl_open(dev, ppa, &fd)) {
		if (GOT_ERR != dl_bind(fd, INSAP, mac_addr)) {
			bcopy( mac_addr, addr, 6);
			close(fd);
			return 0;
		}
	}
	close(fd);
	return -1;
}


int ReadNicMac(int nPos, char *szNicMac, int log_view)
{
	int i = 0;
	int ppa;
	u_char szTmpMac[51];
	char **dev;

	strcpy_st(szNicMac, "");
	for(dev = dlpi_dev, i = 0; **dev != '\0'; ++dev){
		for (ppa=0; ppa<10; ++ppa) {
			memset_st(&szTmpMac, 0, 51);
			if(mac_addr_dlpi(*dev, szTmpMac, ppa) == -1) continue;
			else{
				if(i++ == nPos){
					sprintf_st(szNicMac, "%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x", szTmpMac[0], szTmpMac[1], szTmpMac[2], szTmpMac[3], szTmpMac[4], szTmpMac[5]);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

#endif



