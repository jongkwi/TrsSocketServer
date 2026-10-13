#ifndef ___ERROR_H___
#define ___ERROR_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif


#ifdef _WIN32
#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */
#endif


class CError
{

// Operations
public:
	CError();
	virtual ~CError();

	void err_ret(const char *fmt, ...);
	void err_sys(const char *fmt, ...);
	void err_dump(const char *fmt, ...);
	void err_msg(const char *fmt, ...);
	void err_quit(const char *fmt, ...);
	void err_doit(int, int, const char *, va_list);

protected:

};

#endif//error.h


