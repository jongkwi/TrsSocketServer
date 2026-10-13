#ifndef ___READNICMAC_H___
#define ___READNICMAC_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

#define inaddrr(x) (*(struct in_addr *) &ifr->x[sizeof sa.sin_port])
#define IFRSIZE   ((int)(size * sizeof (struct ifreq)))

typedef struct{
	USHORT		source;		// 출발지 포트
	USHORT		dest;		// 목적지포트	
	USHORT		len;		// 패킷 길이
	USHORT		check;		// 체크섬
}UDP_HEADER;

typedef struct{	
#ifdef _FREEBSD
	BYTE		header_len:4;	// IP header len
	BYTE		version:4;		// IP version	
#else
	BYTE		version:4;		// IP version	
	BYTE		header_len:4;	// IP header len
#endif
	BYTE		tos;			//tos;				// 서비스 클래스 지정
	USHORT		tot_len;		//total_len;		// 패킷의 총 길이
	USHORT		id;				//ident;			// 유일한 식별자
	USHORT		frag_off;		//flags;			// 상태 플래그
	BYTE		ttl;			//ttl;				// Time To Live
	BYTE		protocol;		//proto;			// 프로토콜 종류(예: 1(ICMPIP), 6(TCP), 17(UDP) 등등)
	USHORT		check;			//checksum;			// 체크섬
	ULONG		saddr;			//sourceIP;			// 출발지 아이피
	ULONG		daddr;			//destIP;			// 목적지 아이피
}IP_HEADER;

typedef struct{
	u_short hwType;
	u_short protType;
	u_char hwAddrLen;
	u_char protoAddrLen;
	u_short opcode;
	u_char srcEthAddr[6];
	u_char srcIPAddr[4];
	u_char dstEthAddr[6];
	u_char dstIPAddr[4];
}ARP_HEADER;

typedef struct{
	BYTE	h_dest[6];
	BYTE	h_source[6];
	USHORT	h_proto;
}ETH_HEADER;

typedef struct{
	struct  timeval bh_tstamp;
	UINT    bh_caplen;
	UINT	bh_datalen;
	USHORT	bh_hdrlen;
}BPF_HEADER;

#ifdef _FREEBSD
#define NIC_DEV		"/dev/bpf"
#else
#define NIC_DEV		"/dev/bpf"
#endif

int ReadNicMac(int nPos, char *szNicMac, int log_view);

#endif//ReadNicMac.h


