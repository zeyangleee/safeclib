/*------------------------------------------------------------------
 * test_swscanf_s
 *
 *------------------------------------------------------------------
 */

#include "test_private.h"
#include "safe_str_lib.h"
#include <stdarg.h>

#define LEN   ( 128 )

static wchar_t   wstr1[LEN];
static wchar_t   wstr2[LEN];
static char      str3[LEN];
 
int test_swscanf_s (void)
{
    errno_t rc;
    int32_t  ind;
    size_t  len1;
    size_t  len2;
    size_t  len3;
    int errs = 0;

/*--------------------------------------------------*/

    rc = swscanf_s(wstr1, NULL, NULL);
    ERREOF(ESNULLP);

/*--------------------------------------------------*/

    wstr2[0] = '\0';
    rc = swscanf_s(NULL, L"%ls", wstr2);
    ERREOF(ESNULLP);

/*--------------------------------------------------*/

    wcscpy(wstr1, L"      24");
    rc = swscanf_s(wstr1, L"%ls %n", wstr2, LEN, &ind);
    ERREOF(EINVAL);

    rc = swscanf_s(wstr1, L"%%n");
    ERR(0);
    ERRNO(0);

    rc = swscanf_s(wstr1, L"%ls %%n", wstr2);
    ERR(1);
    ERRNO(0);

    rc = swscanf_s(wstr1, L"%ls %%n", wstr2, 6);
    ERR(1);
    ERRNO(0);

    rc = swscanf_s(wstr1, L"%s %%n", str3, 6);
    ERR(1);
    ERRNO(0);
    EXPSTR(str3, "24");

    rc = swscanf_s(wstr1, L"%%n");
    ERR(0);
    ERRNO(0);

    rc = swscanf_s(wstr1, L" %d", &len1);
    ERR(1);
    ERRNO(0);
    if ((int)len1 != 24) {
        debug_printf("%s %u wrong arg: %d\n",
                     __FUNCTION__, __LINE__, (int)len1);
        errs++;
    }

/*--------------------------------------------------*/

    /* TODO
    rc = swscanf_s(wstr1, L"%s", NULL);
    ERR(ESNULLP)
    */

/*--------------------------------------------------*/

    wcscpy(wstr1, L"aaaaaaaaaa");
    len1 = wcslen(wstr1);

    rc = swscanf_s(wstr1, L"%ls", wstr2, LEN);
    ERR(1)
    len2 = wcslen(wstr2);
    len3 = wcslen(wstr1);
    if (len3 != len2) {
#ifdef DEBUG
        size_t len1 = wcslen(wstr1);
#endif
        debug_printf("%s %u lengths wrong: %lu  %lu  %lu \n",
                     __FUNCTION__, __LINE__, len1, len2, len3);
        errs++;
    }

/*--------------------------------------------------*/

    wcscpy(wstr1, L"keep it simple");

    rc = swscanf_s(wstr1, L"%ls", wstr2, LEN);
    ERR(1);
    WEXPSTR(wstr1, L"keep it simple")

/*--------------------------------------------------*/

    wstr1[0] = '\0';
    wstr2[0] = '\0';

    rc = swscanf_s(wstr1, L"%ls", wstr2, LEN);
    ERR(-1)
    WEXPNULL(wstr1)

/*--------------------------------------------------*/

    wstr1[0] = '\0';
    wcscpy(wstr2, L"keep it simple");

    rc = swscanf_s(wstr1, L"%ls", wstr2, LEN);
    ERR(-1)
    WEXPSTR(wstr1, L"")

/*--------------------------------------------------*/

    wcscpy(wstr1, L"qqweqq");
    wcscpy(wstr2, L"keep it simple");

    rc = swscanf_s(wstr1, L"%ls", wstr2);
    NOERR()
    WEXPSTR(wstr1, wstr2);

/*--------------------------------------------------*/

    /* overlapping works fine on darwin, different on linux glibc */
    /*
    wcscpy(wstr1, L"12345678901234567890");

    rc = swscanf_s(wstr1, L"%ls", &wstr1[7]);
    ERR(1);
    WEXPSTR(wstr1, L"123456712345678901234567890");

    wcscpy(wstr1, L"123456789");

    rc = swscanf_s(wstr1, L"%ls", &wstr1[8]);
    ERR(1);
    WEXPSTR(wstr1, L"12345678123456789");
    */

/*--------------------------------------------------*/

    return (errs);
}

#ifndef __KERNEL__
/* simple hack to get this to work for both userspace and Linux kernel,
   until a better solution can be created. */
int main (void)
{
    return (test_swscanf_s());
}
#endif
