/*
 *  Based on findscu.cc (Revision: 1.41, Date: 2004/02/27 12:51:51)
 */

/*
 *  Original copyright header:
 *
 *  Copyright (C) 1994-2004, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmnet
 *  Author:  Andrew Hewett
 *
 *  Purpose: Query/Retrieve Service Class User (C-FIND operation)
 */

#include "voreen/modules/dicom/dicomfindscu.h"

#ifdef WIN32
#define HAVE_SSTREAM_H 1
#define HAVE_CLASS_TEMPLATE 1
#define HAVE_FUNCTION_TEMPLATE 1
//#pragma include_alias( <sstream.h>, <sstream> )
#else
#define HAVE_CONFIG_H

// prevent warnings
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef HAVE_STAT

#include <dcmtk/config/osconfig.h> /* make sure OS specific configuration is included first */
#endif

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
#include <dcmtk/ofstd/ofstdinc.h>

BEGIN_EXTERN_C
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#ifndef VRN_DCMTK_VERSION_355
#include <dcmtk/dcmdata/dcdebug.h>
#endif
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/cmdlnarg.h>
#include <dcmtk/ofstd/ofconapp.h>
#include <dcmtk/dcmdata/dcuid.h>   /* for dcmtk version name */
#include <dcmtk/dcmdata/dcdicent.h>

#include <dcmtk/dcmdata/dcdeftag.h>


#ifdef WITH_ZLIB
#include <zlib.h>     /* for zlibVersion() */
#endif

#ifdef WITH_OPENSSL
#include <dcmtk/dcmtls/tlstrans.h>
#include <dcmtk/dcmtls/tlslayer.h>
#endif

#include "voreen/modules/dicom/dicomfindscu.h"
#include "voreen/modules/dicom/dicomseriesinfo.h"

using std::string;
using std::vector;

// Start anonymous namespace to encapsulate imported code
namespace {

std::vector<voreen::DicomSeriesInfo>* serieslist; // Found series are added here


static OFBool           opt_verbose = OFFalse;
static OFBool           opt_debug = OFFalse;
static OFBool           opt_abortAssociation = OFFalse;
static OFCmdUnsignedInt opt_maxReceivePDULength = ASC_DEFAULTMAXPDU;
static OFCmdUnsignedInt opt_repeatCount = 1;
static OFBool           opt_extractResponsesToFile = OFFalse;
static const char *     opt_abstractSyntax = UID_FINDModalityWorklistInformationModel;
static OFCmdSignedInt   opt_cancelAfterNResponses = -1;
static DcmDataset *     overrideKeys = NULL;

typedef struct {
    T_ASC_Association *assoc;
    T_ASC_PresentationContextID presId;
} MyCallbackInfo;

static void
errmsg(const char *msg,...)
{
    va_list args;

    fprintf(stderr, "%s: ", "DicomFindSCU");
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
}


#ifndef VRN_DCMTK_VERSION_355
static void
addOverrideKey(/*OFConsoleApplication& app,*/ const char* s)
{
    unsigned int g = 0xffff;
    unsigned int e = 0xffff;
    int n = 0;
    char val[1024];
    OFString msg;
    char msg2[200];

    val[0] = '\0';
    n = sscanf(s, "%x,%x=%s", &g, &e, val);

    if (n < 2) {
      msg = "bad key format: ";
      msg += s;
      errmsg(msg.c_str());
    }

    const char* spos = s;
    char ccc;
    do
    {
      ccc = *spos;
      if (ccc == '=') break;
      if (ccc == 0) { spos = NULL; break; }
      spos++;
    } while(1);

    if (spos && *(spos+1)) {
        strcpy(val, spos+1);
    }

    DcmTag tag(g,e);
    if (tag.error() != EC_Normal) {
        sprintf(msg2, "unknown tag: (%04x,%04x)", g, e);
        errmsg(msg2);
    }
    DcmElement *elem = newDicomElement(tag);
    if (elem == NULL) {
        sprintf(msg2, "cannot create element for tag: (%04x,%04x)", g, e);
        errmsg(msg2);
    }
    if (strlen(val) > 0) {
        elem->putString(val);
        if (elem->error() != EC_Normal)
        {
            sprintf(msg2, "cannot put tag value: (%04x,%04x)=\"", g, e);
            msg = msg2;
            msg += val;
            msg += "\"";
            errmsg(msg.c_str());
        }
    }

    if (overrideKeys == NULL) overrideKeys = new DcmDataset;
    overrideKeys->insert(elem, OFTrue);
    if (overrideKeys->error() != EC_Normal) {
        sprintf(msg2, "cannot insert tag: (%04x,%04x)", g, e);
        errmsg(msg2);
    }
}
#else
static void
addOverrideKey(/*OFConsoleApplication& app,*/ const char* /*s*/) {
    LWARNINGC("dicomfindscu.cpp",
        "addOverrideKey not supported for Dcmtk version " << OFFIS_DCMTK_VERSION_NUMBER);
}
#endif

static OFCondition
addPresentationContexts(T_ASC_Parameters *params);

static OFCondition
cfind(T_ASC_Association *assoc, const char* fname);

#define SHORTCOL 4
#define LONGCOL 16

#if 0
int
main(int argc, char *argv[])
{
    T_ASC_Network *net;
    T_ASC_Parameters *params;
    const char *opt_peer;
    OFCmdUnsignedInt opt_port = 104;
    DIC_NODENAME localHost;
    DIC_NODENAME peerHost;
    T_ASC_Association *assoc;
    const char *opt_peerTitle = PEERAPPLICATIONTITLE;
    const char *opt_ourTitle = APPLICATIONTITLE;
    OFList<OFString> fileNameList;
    OFBool      opt_secureConnection    = OFFalse; /* default: no secure connection */

#ifdef WITH_OPENSSL
    int         opt_keyFileFormat = SSL_FILETYPE_PEM;
    OFBool      opt_doAuthenticate = OFFalse;
    const char *opt_privateKeyFile = NULL;
    const char *opt_certificateFile = NULL;
    const char *opt_passwd = NULL;
    OFString    opt_ciphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
    const char *opt_readSeedFile = NULL;
    const char *opt_writeSeedFile = NULL;
    DcmCertificateVerification opt_certVerification = DCV_requireCertificate;
    const char *opt_dhparam = NULL;
#endif

    /*
    ** Don't let dcmdata remove tailing blank padding or perform other
    ** maipulations.  We want to see the real data.
    */
    dcmEnableAutomaticInputDataCorrection.set(OFFalse);

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif


  char tempstr[20];
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM query (C-FIND) SCU", rcsid);
  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL+SHORTCOL+4);
  cmd.addParam("peer", "hostname of DICOM peer");
  cmd.addParam("port", "tcp/ip port number of peer");
  cmd.addParam("dcmfile_in", "DICOM query file(s)", OFCmdParam::PM_MultiOptional);

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL+2);
   cmd.addOption("--help",                      "-h",        "print this help text and exit");
   cmd.addOption("--version",                                "print version information and exit", OFTrue /* exclusive */);
   cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
   cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");
  cmd.addGroup("network options:");
    cmd.addSubGroup("override matching keys:");
      cmd.addOption("--key",                    "-k",    1,  "key: gggg,eeee=\"string\"", "override matching key");
    cmd.addSubGroup("query information model:");
      cmd.addOption("--worklist",               "-W",        "use modality worklist information model (default)");
      cmd.addOption("--patient",                "-P",        "use patient root information model");
      cmd.addOption("--study",                  "-S",        "use study root information model");
      cmd.addOption("--psonly",                 "-O",        "use patient/study only information model");
    cmd.addSubGroup("application entity titles:");
      OFString opt1 = "set my calling AE title (default: ";
      opt1 += APPLICATIONTITLE;
      opt1 += ")";
      cmd.addOption("--aetitle",                "-aet",  1,  "aetitle: string", opt1.c_str());
      OFString opt2 = "set called AE title of peer (default: ";
      opt2 += PEERAPPLICATIONTITLE;
      opt2 += ")";
      cmd.addOption("--call",                   "-aec",  1,  "aetitle: string", opt2.c_str());
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",          "+u",        "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",         "-u",        "disable support for new VRs, convert to OB");
    cmd.addSubGroup("other network options:");
      OFString opt3 = "set max receive pdu to n bytes (default: ";
      sprintf(tempstr, "%ld", (long)ASC_DEFAULTMAXPDU);
      opt3 += tempstr;
      opt3 += ")";
      OFString opt4 = "[n]umber of bytes: integer [";
      sprintf(tempstr, "%ld", (long)ASC_MINIMUMPDUSIZE);
      opt4 += tempstr;
      opt4 += "..";
      sprintf(tempstr, "%ld", (long)ASC_MAXIMUMPDUSIZE);
      opt4 += tempstr;
      opt4 += "]";
      cmd.addOption("--timeout",                "-to",   1, "[s]econds: integer (default: unlimited)", "timeout for connection requests");
      cmd.addOption("--max-pdu",                "-pdu",  1,  opt4.c_str(), opt3.c_str());
      cmd.addOption("--repeat",                          1,  "[n]umber: integer", "repeat n times");
      cmd.addOption("--abort",                               "abort association instead of releasing it");
      cmd.addOption("--cancel",                          1,  "[n]umber: integer",
                                                             "cancel after n responses (default: never)");
      cmd.addOption("--extract",                "-X",        "extract responses to file (rsp0001.dcm, ...)");

#ifdef WITH_OPENSSL
  cmd.addGroup("transport layer security (TLS) options:");
    cmd.addSubGroup("transport protocol stack options:");
      cmd.addOption("--disable-tls",            "-tls",      "use normal TCP/IP connection (default)");
      cmd.addOption("--enable-tls",             "+tls",  2,  "[p]rivate key file, [c]ertificate file: string",
                                                             "use authenticated secure TLS connection");
      cmd.addOption("--anonymous-tls",          "+tla",      "use secure TLS connection without certificate");
    cmd.addSubGroup("private key password options (only with --enable-tls):");
      cmd.addOption("--std-passwd",             "+ps",       "prompt user to type password on stdin (default)");
      cmd.addOption("--use-passwd",             "+pw",   1,  "[p]assword: string ",
                                                             "use specified password");
      cmd.addOption("--null-passwd",            "-pw",       "use empty string as password");
    cmd.addSubGroup("key and certificate file format options:");
      cmd.addOption("--pem-keys",               "-pem",      "read keys and certificates as PEM file (default)");
      cmd.addOption("--der-keys",               "-der",      "read keys and certificates as DER file");
    cmd.addSubGroup("certification authority options:");
      cmd.addOption("--add-cert-file",          "+cf",   1,  "[c]ertificate filename: string",
                                                             "add certificate file to list of certificates");
      cmd.addOption("--add-cert-dir",           "+cd",   1,  "[c]ertificate directory: string",
                                                             "add certificates in d to list of certificates");
    cmd.addSubGroup("ciphersuite options:");
      cmd.addOption("--cipher",                 "+cs",   1,  "[c]iphersuite name: string",
                                                             "add ciphersuite to list of negotiated suites");
      cmd.addOption("--dhparam",                "+dp",   1,  "[f]ilename: string",
                                                             "read DH parameters for DH/DSS ciphersuites");
    cmd.addSubGroup("pseudo random generator options:");
      cmd.addOption("--seed",                   "+rs",   1,  "[f]ilename: string",
                                                             "seed random generator with contents of f");
      cmd.addOption("--write-seed",             "+ws",       "write back modified seed (only with --seed)");
      cmd.addOption("--write-seed-file",        "+wf",   1,  "[f]ilename: string (only with --seed)",
                                                             "write modified seed to file f");
    cmd.addSubGroup("peer authentication options:");
      cmd.addOption("--require-peer-cert",      "-rc",       "verify peer certificate, fail if absent (default)");
      cmd.addOption("--verify-peer-cert",       "-vc",       "verify peer certificate if present");
      cmd.addOption("--ignore-peer-cert",       "-ic",       "don't verify peer certificate");
#endif

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      /* check exclusive options first */

      if (cmd.getParamCount() == 0)
      {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);          // uses ofConsole.lockCerr()
            CERR << endl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(WITH_OPENSSL)
              CERR << " none" << endl;
#else
              CERR << endl;
#endif
#ifdef WITH_ZLIB
              CERR << "- ZLIB, Version " << zlibVersion() << endl;
#endif
#ifdef WITH_OPENSSL
              CERR << "- " << OPENSSL_VERSION_TEXT << endl;
#endif
            return 0;
         }
      }

      /* command line parameters */

      cmd.getParam(1, opt_peer);
      app.checkParam(cmd.getParamAndCheckMinMax(2, opt_port, 1, 65535));

      if (cmd.findOption("--verbose")) opt_verbose=OFTrue;
      if (cmd.findOption("--debug"))
      {
        opt_debug = OFTrue;
        DUL_Debug(OFTrue);
        DIMSE_debug(OFTrue);
        SetDebugLevel(3);
      }

      if (cmd.findOption("--key", 0, OFCommandLine::FOM_First))
      {
        const char *ovKey = NULL;
        do {
          app.checkValue(cmd.getValue(ovKey));
          addOverrideKey(app, ovKey);
        } while (cmd.findOption("--key", 0, OFCommandLine::FOM_Next));
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--worklist")) opt_abstractSyntax = UID_FINDModalityWorklistInformationModel;
      if (cmd.findOption("--patient"))  opt_abstractSyntax = UID_FINDPatientRootQueryRetrieveInformationModel;
      if (cmd.findOption("--study"))    opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;
      if (cmd.findOption("--psonly"))   opt_abstractSyntax = UID_FINDPatientStudyOnlyQueryRetrieveInformationModel;
      cmd.endOptionBlock();
      if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_ourTitle));
      if (cmd.findOption("--call")) app.checkValue(cmd.getValue(opt_peerTitle));
      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFTrue);
        dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
      }
      if (cmd.findOption("--disable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
      }
      cmd.endOptionBlock();

      if (cmd.findOption("--timeout"))
      {
        OFCmdSignedInt opt_timeout = 0;
        app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
        dcmConnectionTimeout.set((Sint32) opt_timeout);
      }

      if (cmd.findOption("--max-pdu")) app.checkValue(cmd.getValueAndCheckMinMax(opt_maxReceivePDULength, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
      if (cmd.findOption("--repeat"))  app.checkValue(cmd.getValueAndCheckMin(opt_repeatCount, 1));
      if (cmd.findOption("--abort"))   opt_abortAssociation = OFTrue;
      if (cmd.findOption("--cancel"))  app.checkValue(cmd.getValueAndCheckMin(opt_cancelAfterNResponses, 0));
      if (cmd.findOption("--extract")) opt_extractResponsesToFile = OFTrue;

      /* finally parse filenames */
      int paramCount = cmd.getParamCount();
      const char *currentFilename = NULL;
      OFString errormsg;

      for (int i=3; i <= paramCount; i++)
      {
        cmd.getParam(i, currentFilename);
        if (access(currentFilename, R_OK) < 0)
        {
          errormsg = "cannot access file: ";
          errormsg += currentFilename;
          app.printError(errormsg.c_str());
        }
        fileNameList.push_back(currentFilename);
      }

      if ((fileNameList.empty()) && (overrideKeys == NULL))
      {
          app.printError("either query file or override keys (or both) must be specified");
      }

#ifdef WITH_OPENSSL

#ifdef DEBUG
      /* prevent command line code from moaning that --add-cert-dir and --add-cert-file have not been checked */
      if (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_First)) /* nothing */ ;
      if (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_First)) /* nothing */ ;
#endif

      cmd.beginOptionBlock();
      if (cmd.findOption("--disable-tls")) opt_secureConnection = OFFalse;
      if (cmd.findOption("--enable-tls"))
      {
        opt_secureConnection = OFTrue;
        opt_doAuthenticate = OFTrue;
        app.checkValue(cmd.getValue(opt_privateKeyFile));
        app.checkValue(cmd.getValue(opt_certificateFile));
      }
      if (cmd.findOption("--anonymous-tls"))
      {
        opt_secureConnection = OFTrue;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--std-passwd"))
      {
        if (! opt_doAuthenticate) app.printError("--std-passwd only with --enable-tls");
        opt_passwd = NULL;
      }
      if (cmd.findOption("--use-passwd"))
      {
        if (! opt_doAuthenticate) app.printError("--use-passwd only with --enable-tls");
        app.checkValue(cmd.getValue(opt_passwd));
      }
      if (cmd.findOption("--null-passwd"))
      {
        if (! opt_doAuthenticate) app.printError("--null-passwd only with --enable-tls");
        opt_passwd = "";
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--pem-keys")) opt_keyFileFormat = SSL_FILETYPE_PEM;
      if (cmd.findOption("--der-keys")) opt_keyFileFormat = SSL_FILETYPE_ASN1;
      cmd.endOptionBlock();

      if (cmd.findOption("--dhparam"))
      {
        app.checkValue(cmd.getValue(opt_dhparam));
      }

      if (cmd.findOption("--seed"))
      {
        app.checkValue(cmd.getValue(opt_readSeedFile));
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-seed"))
      {
        if (opt_readSeedFile == NULL) app.printError("--write-seed only with --seed");
        opt_writeSeedFile = opt_readSeedFile;
      }
      if (cmd.findOption("--write-seed-file"))
      {
        if (opt_readSeedFile == NULL) app.printError("--write-seed-file only with --seed");
        app.checkValue(cmd.getValue(opt_writeSeedFile));
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--require-peer-cert")) opt_certVerification = DCV_requireCertificate;
      if (cmd.findOption("--verify-peer-cert"))  opt_certVerification = DCV_checkCertificate;
      if (cmd.findOption("--ignore-peer-cert"))  opt_certVerification = DCV_ignoreCertificate;
      cmd.endOptionBlock();

      const char *current = NULL;
      const char *currentOpenSSL;
      if (cmd.findOption("--cipher", 0, OFCommandLine::FOM_First))
      {
        opt_ciphersuites.clear();
        do
        {
          app.checkValue(cmd.getValue(current));
          if (NULL == (currentOpenSSL = DcmTLSTransportLayer::findOpenSSLCipherSuiteName(current)))
          {
            CERR << "ciphersuite '" << current << "' is unknown. Known ciphersuites are:" << endl;
            unsigned long numSuites = DcmTLSTransportLayer::getNumberOfCipherSuites();
            for (unsigned long cs=0; cs < numSuites; cs++)
            {
              CERR << "    " << DcmTLSTransportLayer::getTLSCipherSuiteName(cs) << endl;
            }
            return 1;
          } else {
            if (opt_ciphersuites.length() > 0) opt_ciphersuites += ":";
            opt_ciphersuites += currentOpenSSL;
          }
        } while (cmd.findOption("--cipher", 0, OFCommandLine::FOM_Next));
      }

#endif

   }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded()) {
        fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
                DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* initialize network, i.e. create an instance of T_ASC_Network*. */
    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 1000, &net);
    if (cond.bad()) {
        DimseCondition::dump(cond);
        return 1;
    }

#ifdef WITH_OPENSSL

    DcmTLSTransportLayer *tLayer = NULL;
    if (opt_secureConnection)
    {
      tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_REQUESTOR, opt_readSeedFile);
      if (tLayer == NULL)
      {
        app.printError("unable to create TLS transport layer");
      }

      if (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_First))
      {
        const char *current = NULL;
        do
        {
          app.checkValue(cmd.getValue(current));
          if (TCS_ok != tLayer->addTrustedCertificateFile(current, opt_keyFileFormat))
          {
            CERR << "warning unable to load certificate file '" << current << "', ignoring" << endl;
          }
        } while (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_Next));
      }

      if (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_First))
      {
        const char *current = NULL;
        do
        {
          app.checkValue(cmd.getValue(current));
          if (TCS_ok != tLayer->addTrustedCertificateDir(current, opt_keyFileFormat))
          {
            CERR << "warning unable to load certificates from directory '" << current << "', ignoring" << endl;
          }
        } while (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_Next));
      }

      if (opt_dhparam && ! (tLayer->setTempDHParameters(opt_dhparam)))
      {
        CERR << "warning unable to load temporary DH parameter file '" << opt_dhparam << "', ignoring" << endl;
      }

      if (opt_doAuthenticate)
      {
        if (opt_passwd) tLayer->setPrivateKeyPasswd(opt_passwd);

        if (TCS_ok != tLayer->setPrivateKeyFile(opt_privateKeyFile, opt_keyFileFormat))
        {
          CERR << "unable to load private TLS key from '" << opt_privateKeyFile << "'" << endl;
          return 1;
        }
        if (TCS_ok != tLayer->setCertificateFile(opt_certificateFile, opt_keyFileFormat))
        {
          CERR << "unable to load certificate from '" << opt_certificateFile << "'" << endl;
          return 1;
        }
        if (! tLayer->checkPrivateKeyMatchesCertificate())
        {
          CERR << "private key '" << opt_privateKeyFile << "' and certificate '" << opt_certificateFile << "' do not match" << endl;
          return 1;
        }
      }

      if (TCS_ok != tLayer->setCipherSuites(opt_ciphersuites.c_str()))
      {
        CERR << "unable to set selected cipher suites" << endl;
        return 1;
      }

      tLayer->setCertificateVerification(opt_certVerification);


      cond = ASC_setTransportLayer(net, tLayer, 0);
      if (cond.bad())
      {
          DimseCondition::dump(cond);
          return 1;
      }
    }

#endif

    /* initialize asscociation parameters, i.e. create an instance of T_ASC_Parameters*. */
    cond = ASC_createAssociationParameters(&params, opt_maxReceivePDULength);
    if (cond.bad()) {
        DimseCondition::dump(cond);
        return 1;
    }

    /* sets this application's title and the called application's title in the params */
    /* structure. The default values to be set here are "STORESCU" and "ANY-SCP". */
    ASC_setAPTitles(params, opt_ourTitle, opt_peerTitle, NULL);

    /* Set the transport layer type (type of network connection) in the params */
    /* structure. The default is an insecure connection; where OpenSSL is  */
    /* available the user is able to request an encrypted,secure connection. */
    cond = ASC_setTransportLayerType(params, opt_secureConnection);
    if (cond.bad()) {
        DimseCondition::dump(cond);
        return 1;
    }

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", opt_peer, (int)opt_port);
    ASC_setPresentationAddresses(params, localHost, peerHost);

    /* Set the presentation contexts which will be negotiated */
    /* when the network connection will be established */
    cond = addPresentationContexts(params);
    if (cond.bad()) {
        DimseCondition::dump(cond);
        return 1;
    }

    /* dump presentation contexts if required */
    if (opt_debug) {
        printf("Request Parameters:\n");
        ASC_dumpParameters(params, COUT);
    }

    /* create association, i.e. try to establish a network connection to another */
    /* DICOM application. This call creates an instance of T_ASC_Association*. */
    if (opt_verbose)
        printf("Requesting Association\n");
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad()) {
        if (cond == DUL_ASSOCIATIONREJECTED) {
            T_ASC_RejectParameters rej;

            ASC_getRejectParameters(params, &rej);
            errmsg("Association Rejected:");
            ASC_printRejectParameters(stderr, &rej);
            return 1;
        } else {
            errmsg("Association Request Failed:");
            DimseCondition::dump(cond);
            return 1;
        }
    }

    /* dump the presentation contexts which have been accepted/refused */
    if (opt_debug) {
        printf("Association Parameters Negotiated:\n");
        ASC_dumpParameters(params, COUT);
    }

    /* count the presentation contexts which have been accepted by the SCP */
    /* If there are none, finish the execution */
    if (ASC_countAcceptedPresentationContexts(params) == 0) {
        errmsg("No Acceptable Presentation Contexts");
        return 1;
    }

    /* dump general information concerning the establishment of the network connection if required */
    if (opt_verbose) {
        printf("Association Accepted (Max Send PDV: %lu)\n",
                assoc->sendPDVLength);
    }

    /* do the real work, i.e. for all files which were specified in the command line, send a */
    /* C-FIND-RQ to the other DICOM application and receive corresponding response messages. */
    cond = EC_Normal;
    if (fileNameList.empty())
    {
        /* no files provided on command line */
        cond = cfind(assoc, NULL);
    } else {
      OFListIterator(OFString) iter = fileNameList.begin();
      OFListIterator(OFString) enditer = fileNameList.end();
      while ((iter != enditer) && (cond == EC_Normal)) // compare with EC_Normal since DUL_PEERREQUESTEDRELEASE is also good()
      {
          cond = cfind(assoc, (*iter).c_str());
          ++iter;
      }
    }

    /* tear down association, i.e. terminate network connection to SCP */
    if (cond == EC_Normal)
    {
        if (opt_abortAssociation) {
            if (opt_verbose)
                printf("Aborting Association\n");
            cond = ASC_abortAssociation(assoc);
            if (cond.bad()) {
                errmsg("Association Abort Failed:");
                DimseCondition::dump(cond);
                return 1;
            }
        } else {
            /* release association */
            if (opt_verbose)
                printf("Releasing Association\n");
            cond = ASC_releaseAssociation(assoc);
            if (cond.bad())
            {
                errmsg("Association Release Failed:");
                DimseCondition::dump(cond);
                return 1;
            }
        }
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        errmsg("Protocol Error: peer requested release (Aborting)");
        if (opt_verbose)
            printf("Aborting Association\n");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            errmsg("Association Abort Failed:");
            DimseCondition::dump(cond);
            return 1;
        }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        if (opt_verbose) printf("Peer Aborted Association\n");
    }
    else
    {
        errmsg("SCU Failed:");
        DimseCondition::dump(cond);
        if (opt_verbose)
            printf("Aborting Association\n");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            errmsg("Association Abort Failed:");
            DimseCondition::dump(cond);
            return 1;
        }
    }

    /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
    /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad()) {
        DimseCondition::dump(cond);
        return 1;
    }

    /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
    /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
    cond = ASC_dropNetwork(&net);
    if (cond.bad()) {
        DimseCondition::dump(cond);
        return 1;
    }

#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

#ifdef WITH_OPENSSL
    if (tLayer && opt_writeSeedFile)
    {
      if (tLayer->canWriteRandomSeed())
      {
        if (!tLayer->writeRandomSeed(opt_writeSeedFile))
        {
          CERR << "Error while writing random seed file '" << opt_writeSeedFile << "', ignoring." << endl;
        }
      } else {
        CERR << "Warning: cannot write random seed, ignoring." << endl;
      }
    }
    delete tLayer;
#endif

    delete overrideKeys;
    overrideKeys = 0;
    return 0;
}
#endif // 0


static OFCondition
addPresentationContexts(T_ASC_Parameters *params)
{
    OFCondition cond = EC_Normal;

    /*
    ** We prefer to accept Explicitly encoded transfer syntaxes.
    ** If we are running on a Little Endian machine we prefer
    ** LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
    ** Some SCP implementations will just select the first transfer
    ** syntax they support (this is not part of the standard) so
    ** organise the proposed transfer syntaxes to take advantage
    ** of such behaviour.
    */

    const char* transferSyntaxes[] = {
        NULL, NULL, UID_LittleEndianImplicitTransferSyntax };

    /* gLocalByteOrder is defined in dcxfer.h */
    if (gLocalByteOrder == EBO_LittleEndian) {
        /* we are on a little endian machine */
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
    } else {
        /* we are on a big endian machine */
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
    }

    cond = ASC_addPresentationContext(
        params, 1, opt_abstractSyntax,
        transferSyntaxes, DIM_OF(transferSyntaxes));

    return cond;
}

static void
substituteOverrideKeys(DcmDataset *dset)
{
    if (overrideKeys == NULL) {
        return; /* nothing to do */
    }

    /* copy the override keys */
    DcmDataset keys(*overrideKeys);

    /* put the override keys into dset replacing existing tags */
    unsigned long elemCount = keys.card();
    for (unsigned long i=0; i<elemCount; i++) {
        DcmElement *elem = keys.remove((unsigned long)0);

        dset->insert(elem, OFTrue);
    }
}

static OFBool writeToFile(const char* ofname, DcmDataset *dataset)
{
    /* write out as a file format */

    DcmFileFormat fileformat(dataset); // copies dataset
    OFCondition ec = fileformat.error();
    if (ec.bad()) {
        errmsg("error writing file: %s: %s", ofname, ec.text());
        return OFFalse;
    }

    ec = fileformat.saveFile(ofname, dataset->getOriginalXfer());
    if (ec.bad()) {
        errmsg("error writing file: %s: %s", ofname, ec.text());
        return OFFalse;
    }

    return OFTrue;
}

static void
progressCallback(
        void *callbackData,
        T_DIMSE_C_FindRQ *request,
        int responseCount,
        T_DIMSE_C_FindRSP * /*rsp*/,
        DcmDataset *responseIdentifiers
        )
    /*
     * This function.is used to indicate progress when findscu receives search results over the
     * network. This function will simply cause some information to be dumped to stdout.
     *
     * Parameters:
     *   callbackData        - [in] data for this callback function
     *   request             - [in] The original find request message.
     *   responseCount       - [in] Specifies how many C-FIND-RSP were received including the current one.
     *   rsp                 - [in] the C-FIND-RSP message which was received shortly before the call to
     *                              this function.
     *   responseIdentifiers - [in] Contains the record which was received. This record matches the search
     *                              mask of the C-FIND-RQ which was sent.
     */
{
    /* dump response number */
//    printf("RESPONSE: %d (%s)\n", responseCount,
//        DU_cfindStatusString(rsp->DimseStatus));

    /* dump data set which was received */
//    responseIdentifiers->print(COUT);

    OFString tmpString;
    voreen::DicomSeriesInfo tmp;
    if (responseIdentifiers->findAndGetOFString(DCM_SeriesInstanceUID, tmpString).good()) {
        tmp.uid_ = tmpString.c_str();
        if (responseIdentifiers->findAndGetOFString(DCM_PatientsName, tmpString, 0, OFTrue).good())
            tmp.patientsName_ = tmpString.c_str();
        if (responseIdentifiers->findAndGetOFString(DCM_PatientID, tmpString, 0, OFTrue).good()) {
           tmp.patientId_ = tmpString.c_str();
        }
        if (responseIdentifiers->findAndGetOFString(DCM_StudyDate, tmpString, 0, OFTrue).good()) {
            tmp.studyDate_ = tmpString.c_str();
        }
        if (responseIdentifiers->findAndGetOFString(DCM_StudyTime, tmpString, 0, OFTrue).good()) {
            tmp.studyTime_ = tmpString.c_str();
        }
        if (responseIdentifiers->findAndGetOFString(DCM_Modality, tmpString, 0, OFTrue).good()) {
            tmp.modality_ = tmpString.c_str();
        }
        if (responseIdentifiers->findAndGetOFString(DCM_NumberOfSeriesRelatedInstances, tmpString, 0, OFTrue).good()) {
            tmp.numImages_ = tmpString.c_str();
        }
        serieslist->push_back(tmp);
    }

    /* dump delimiter */
//    printf("--------\n");

    /* in case opt_extractResponsesToFile is set the responses shall be extracted to a certain file */
    if (opt_extractResponsesToFile) {
        char rspIdsFileName[1024];
        sprintf(rspIdsFileName, "rsp%04d.dcm", responseCount);
        writeToFile(rspIdsFileName, responseIdentifiers);
    }

    MyCallbackInfo *myCallbackData = OFstatic_cast(MyCallbackInfo *, callbackData);

    /* should we send a cancel back ?? */
    if (opt_cancelAfterNResponses == responseCount)
    {
        if (opt_verbose)
        {
            printf("Sending Cancel RQ, MsgId: %d, PresId: %d\n", request->MessageID, myCallbackData->presId);
        }
        OFCondition cond = DIMSE_sendCancelRequest(myCallbackData->assoc, myCallbackData->presId, request->MessageID);
        if (cond.bad())
        {
            errmsg("Cancel RQ Failed:");
            DimseCondition::dump(cond);
        }
    }

}

static OFCondition
findSCU(T_ASC_Association * assoc, const char *fname)
    /*
     * This function will read all the information from the given file
     * (this information specifies a search mask), figure out a corresponding
     * presentation context which will be used to transmit a C-FIND-RQ message
     * over the network to the SCP, and it will finally initiate the transmission
     * of data to the SCP.
     *
     * Parameters:
     *   assoc - [in] The association (network connection to another DICOM application).
     *   fname - [in] Name of the file which shall be processed.
     */
{
    DIC_US msgId = assoc->nextMsgID++;
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_FindRQ req;
    T_DIMSE_C_FindRSP rsp;
    DcmDataset *statusDetail = NULL;
    MyCallbackInfo callbackData;
    DcmFileFormat dcmff;

    /* if there is a valid filename */
    if (fname != NULL) {

        /* read information from file (this information specifies a search mask). After the */
        /* call to DcmFileFormat::read(...) the information which is encapsulated in the file */
        /* will be available through the DcmFileFormat object. In detail, it will be available */
        /* through calls to DcmFileFormat::getMetaInfo() (for meta header information) and */
        /* DcmFileFormat::getDataset() (for data set information). */
        OFCondition cond = dcmff.loadFile(fname);

        /* figure out if an error occured while the file was read*/
        if (cond.bad()) {
            errmsg("Bad DICOM file: %s: %s", fname, cond.text());
            return cond;
        }
    }

    /* replace specific keys by those in overrideKeys */
    substituteOverrideKeys(dcmff.getDataset());

    /* figure out which of the accepted presentation contexts should be used */
    presId = ASC_findAcceptedPresentationContextID(
        assoc, opt_abstractSyntax);
    if (presId == 0) {
        errmsg("No presentation context");
        return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    /* prepare the transmission of data */
    bzero((char*)&req, sizeof(req));
    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, opt_abstractSyntax);
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;

    /* prepare the callback data */
    callbackData.assoc = assoc;
    callbackData.presId = presId;

    /* if required, dump some more general information */
    if (opt_verbose) {
        printf("Find SCU RQ: MsgID %d\n", msgId);
        printf("REQUEST:\n");
        dcmff.getDataset()->print(COUT);
        printf("--------\n");
    }

    /* finally conduct transmission of data */
    OFCondition cond = DIMSE_findUser(assoc, presId, &req, dcmff.getDataset(),
                          progressCallback, &callbackData,
                          DIMSE_BLOCKING, 0,
                          &rsp, &statusDetail);


    /* dump some more general information */
    if (cond == EC_Normal) {
        if (opt_verbose) {
            DIMSE_printCFindRSP(stdout, &rsp);
        } else {
            if (rsp.DimseStatus != STATUS_Success) {
                printf("Response: %s\n", DU_cfindStatusString(rsp.DimseStatus));
            }
        }
    } else {
        if (fname) {
            errmsg("Find Failed, file: %s:", fname);
        } else {
            errmsg("Find Failed, query keys:");
            dcmff.getDataset()->print(COUT);
        }
        DimseCondition::dump(cond);
    }

    /* dump status detail information if there is some */
    if (statusDetail != NULL) {
        printf("  Status Detail:\n");
        statusDetail->print(COUT);
        delete statusDetail;
    }

    /* return */
    return cond;
}


static OFCondition
cfind(T_ASC_Association * assoc, const char *fname)
    /*
     * This function will process the given file as often as is specified by opt_repeatCount.
     * "Process" in this case means "read file, send C-FIND-RQ, receive C-FIND-RSP messages".
     *
     * Parameters:
     *   assoc - [in] The association (network connection to another DICOM application).
     *   fname - [in] Name of the file which shall be processed (contains search mask information).
     */
{
    OFCondition cond = EC_Normal;

    /* opt_repeatCount specifies how many times a certain file shall be processed */
    int n = (int)opt_repeatCount;

    /* as long as no error occured and the counter does not equal 0 */
    while (cond == EC_Normal && n--) {
        /* process file (read file, send C-FIND-RQ, receive C-FIND-RSP messages) */
        cond = findSCU(assoc, fname);
    }

    /* return result value */
    return cond;
}


} // namespace

int voreen::DicomFindSCU::find(const string &ourTitle, const string &peer,
                               int port, const string &peerTitle, std::vector<DicomSeriesInfo>* series,
                               const DicomSecurityOptions& security) {
    T_ASC_Network *net;
    T_ASC_Parameters *params;
    DIC_NODENAME localHost;
    DIC_NODENAME peerHost;
    T_ASC_Association *assoc;
    OFList<OFString> fileNameList;
    OFBool opt_secureConnection = (security.secureConnection_ ? OFTrue : OFFalse);

    serieslist = series;


#ifdef WITH_OPENSSL
    int         opt_keyFileFormat = SSL_FILETYPE_PEM;
    OFBool      opt_doAuthenticate = (security.authenticate_ ? OFTrue : OFFalse);
    const char *opt_privateKeyFile = (security.privateKeyFile_.empty() ? NULL : security.privateKeyFile_.c_str());
    const char *opt_certificateFile = (security.publicKeyFile_.empty() ? NULL : security.publicKeyFile_.c_str());
    const char *opt_passwd = NULL;
    OFString    opt_ciphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
    const char *opt_readSeedFile = NULL;
    //const char *opt_writeSeedFile = NULL;
    DcmCertificateVerification opt_certVerification = DCV_requireCertificate;
    //const char *opt_dhparam = NULL;
#endif

    /*
    ** Don't let dcmdata remove tailing blank padding or perform other
    ** maipulations.  We want to see the real data.
    */
    dcmEnableAutomaticInputDataCorrection.set(OFFalse);

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif


//  opt_verbose=OFTrue;
  if (0/*cmd.findOption("--debug"*/)
  {
      opt_debug = OFTrue;
#ifndef VRN_DCMTK_VERSION_355
      DUL_Debug(OFTrue);
      DIMSE_debug(OFTrue);
      SetDebugLevel(3);
#endif
  }

    //push back the keys that should be recieved:
  vector<string> keys;
  keys.push_back("0008,0052=SERIES");
  keys.push_back("0010,0010="); //PatientID
  keys.push_back("0010,0020="); //PatientsName
  keys.push_back("0020,000d="); //StudyInstanceUID
  keys.push_back("0020,000e="); //SeriesInstanceUID
  keys.push_back("0008,0020="); //StudyDate
  keys.push_back("0008,0030="); //StudyTime
  keys.push_back("0008,0060="); //Modality
  keys.push_back("0020,1209="); //Number of Series Related Images

  vector<string>::iterator iter = keys.begin();
  while (iter != keys.end()) {
      addOverrideKey((*iter).c_str());
      iter++;
  }
  opt_abstractSyntax = UID_FINDPatientRootQueryRetrieveInformationModel;


  const char *opt_ourTitle = ourTitle.c_str();
  const char *opt_peerTitle = peerTitle.c_str();

  const char *opt_peer = peer.c_str();
  OFCmdUnsignedInt opt_port = port;


  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded()) {
      fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
              DCM_DICT_ENVIRONMENT_VARIABLE);
  }

    /* initialize network, i.e. create an instance of T_ASC_Network*. */
    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 1000, &net);
    if (cond.bad()) {
        DimseCondition::dump(cond);
        return 1;
    }

#ifdef WITH_OPENSSL

    DcmTLSTransportLayer *tLayer = NULL;
    if (opt_secureConnection)
    {
      tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_REQUESTOR, opt_readSeedFile);
      if (tLayer == NULL)
      {
          std::cerr << "unable to create TLS transport layer" << std::endl;
      }

      for (size_t i=0; i < security.certificateFiles_.size(); i++)
          tLayer->addTrustedCertificateFile(security.certificateFiles_[i].c_str(), opt_keyFileFormat);

/*
      if (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_First))
      {
        const char *current = NULL;
        do
        {
          app.checkValue(cmd.getValue(current));
          if (TCS_ok != tLayer->addTrustedCertificateFile(current, opt_keyFileFormat))
          {
            CERR << "warning unable to load certificate file '" << current << "', ignoring" << endl;
          }
        } while (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_Next));
      }

      if (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_First))
      {
        const char *current = NULL;
        do
        {
          app.checkValue(cmd.getValue(current));
          if (TCS_ok != tLayer->addTrustedCertificateDir(current, opt_keyFileFormat))
          {
            CERR << "warning unable to load certificates from directory '" << current << "', ignoring" << endl;
          }
        } while (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_Next));
      }

      if (opt_dhparam && ! (tLayer->setTempDHParameters(opt_dhparam)))
      {
        CERR << "warning unable to load temporary DH parameter file '" << opt_dhparam << "', ignoring" << endl;
      }
*/
      if (opt_doAuthenticate)
      {
        if (opt_passwd) tLayer->setPrivateKeyPasswd(opt_passwd);

        if (TCS_ok != tLayer->setPrivateKeyFile(opt_privateKeyFile, opt_keyFileFormat))
        {
          CERR << "unable to load private TLS key from '" << opt_privateKeyFile << "'" << endl;
          return 1;
        }
        if (TCS_ok != tLayer->setCertificateFile(opt_certificateFile, opt_keyFileFormat))
        {
          CERR << "unable to load certificate from '" << opt_certificateFile << "'" << endl;
          return 1;
        }
        if (! tLayer->checkPrivateKeyMatchesCertificate())
        {
          CERR << "private key '" << opt_privateKeyFile << "' and certificate '" << opt_certificateFile << "' do not match" << endl;
          return 1;
        }
      }

      if (TCS_ok != tLayer->setCipherSuites(opt_ciphersuites.c_str()))
      {
        CERR << "unable to set selected cipher suites" << endl;
        return 1;
      }

      tLayer->setCertificateVerification(opt_certVerification);


      cond = ASC_setTransportLayer(net, tLayer, 0);
      if (cond.bad())
      {
          DimseCondition::dump(cond);
          return 1;
      }
    }

#endif // WITH_OPENSSL

    /* initialize asscociation parameters, i.e. create an instance of T_ASC_Parameters*. */
    cond = ASC_createAssociationParameters(&params, opt_maxReceivePDULength);
    if (cond.bad()) {
        DimseCondition::dump(cond);
        return 1;
    }

    /* sets this application's title and the called application's title in the params */
    /* structure. The default values to be set here are "STORESCU" and "ANY-SCP". */
    ASC_setAPTitles(params, opt_ourTitle, opt_peerTitle, NULL);

    /* Set the transport layer type (type of network connection) in the params */
    /* structure. The default is an insecure connection; where OpenSSL is  */
    /* available the user is able to request an encrypted,secure connection. */
    cond = ASC_setTransportLayerType(params, opt_secureConnection);
    if (cond.bad()) {
        DimseCondition::dump(cond);
        return 1;
    }

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", opt_peer, (int)opt_port);
    ASC_setPresentationAddresses(params, localHost, peerHost);

    /* Set the presentation contexts which will be negotiated */
    /* when the network connection will be established */
    cond = addPresentationContexts(params);
    if (cond.bad()) {
        DimseCondition::dump(cond);
        return 1;
    }

    /* dump presentation contexts if required */
    if (opt_debug) {
        printf("Request Parameters:\n");
        ASC_dumpParameters(params, COUT);
    }

    /* create association, i.e. try to establish a network connection to another */
    /* DICOM application. This call creates an instance of T_ASC_Association*. */
    if (opt_verbose)
        printf("Requesting Association\n");
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad()) {
        if (cond == DUL_ASSOCIATIONREJECTED) {
            T_ASC_RejectParameters rej;

            ASC_getRejectParameters(params, &rej);
            errmsg("Association Rejected:");
            ASC_printRejectParameters(stderr, &rej);
            return 1;
        } else {
            errmsg("Association Request Failed:");
            DimseCondition::dump(cond);
            return 1;
        }
    }

    /* dump the presentation contexts which have been accepted/refused */
    if (opt_debug) {
        printf("Association Parameters Negotiated:\n");
        ASC_dumpParameters(params, COUT);
    }

    /* count the presentation contexts which have been accepted by the SCP */
    /* If there are none, finish the execution */
    if (ASC_countAcceptedPresentationContexts(params) == 0) {
        errmsg("No Acceptable Presentation Contexts");
        return 1;
    }

    /* dump general information concerning the establishment of the network connection if required */
    if (opt_verbose) {
        printf("Association Accepted (Max Send PDV: %lu)\n",
                assoc->sendPDVLength);
    }

    /* do the real work, i.e. for all files which were specified in the command line, send a */
    /* C-FIND-RQ to the other DICOM application and receive corresponding response messages. */
    cond = EC_Normal;
    if (fileNameList.empty())
    {
        /* no files provided on command line */
        cond = cfind(assoc, NULL);
    } else {
      OFListIterator(OFString) iter = fileNameList.begin();
      OFListIterator(OFString) enditer = fileNameList.end();
      while ((iter != enditer) && (cond == EC_Normal)) // compare with EC_Normal since DUL_PEERREQUESTEDRELEASE is also good()
      {
          cond = cfind(assoc, (*iter).c_str());
          ++iter;
      }
    }

    /* tear down association, i.e. terminate network connection to SCP */
    if (cond == EC_Normal)
    {
        if (opt_abortAssociation) {
            if (opt_verbose)
                printf("Aborting Association\n");
            cond = ASC_abortAssociation(assoc);
            if (cond.bad()) {
                errmsg("Association Abort Failed:");
                DimseCondition::dump(cond);
                return 1;
            }
        } else {
            /* release association */
            if (opt_verbose)
                printf("Releasing Association\n");
            cond = ASC_releaseAssociation(assoc);
            if (cond.bad())
            {
                errmsg("Association Release Failed:");
                DimseCondition::dump(cond);
                return 1;
            }
        }
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        errmsg("Protocol Error: peer requested release (Aborting)");
        if (opt_verbose)
            printf("Aborting Association\n");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            errmsg("Association Abort Failed:");
            DimseCondition::dump(cond);
            return 1;
        }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        if (opt_verbose) printf("Peer Aborted Association\n");
    }
    else
    {
        errmsg("SCU Failed:");
        DimseCondition::dump(cond);
        if (opt_verbose)
            printf("Aborting Association\n");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            errmsg("Association Abort Failed:");
            DimseCondition::dump(cond);
            return 1;
        }
    }

    /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
    /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad()) {
        DimseCondition::dump(cond);
        return 1;
    }

    /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
    /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
    cond = ASC_dropNetwork(&net);
    if (cond.bad()) {
        DimseCondition::dump(cond);
        return 1;
    }

#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

#ifdef WITH_OPENSSL
    delete tLayer;
#endif

    delete overrideKeys;
    overrideKeys = 0;
    return 0;
}

int voreen::DicomFindSCU::find(const string &url, vector<DicomSeriesInfo>* series,
                               const DicomSecurityOptions& security)
{
    // First extract the URL parts like in this regex:
    // ^dicom://([a-zA-Z0-9\-.]+)@([a-zA-Z0-9\-\.]+):([0-9]+)/([a-zA-Z0-9\-\.]+)$

    const string number = "0123456789";
    const string word = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-." + number;
    string ourtitle, peer, peertitle;
    int port;
    string s(url);

    // ^dicom://
    if (s.find("dicom://") != 0) return false;
    s = s.substr(8);

    // ([a-zA-Z0-9\-.]+)@
    size_t pos = s.find_first_not_of(word);
    if (pos <= 0) return false;
    ourtitle = s.substr(0, pos);
    if (s.substr(pos, 1) != "@") return false;
    s = s.substr(pos + 1);

    // ([a-zA-Z0-9\-.]+):
    pos = s.find_first_not_of(word);
    if (pos <= 0) return false;
    peer = s.substr(0, pos);
    if (s.substr(pos, 1) != ":") return false;
    s = s.substr(pos + 1);

    // ([0-9]+)
    pos = s.find_first_not_of(number);
    port = atoi(s.substr(0, pos).c_str());

    // /([a-zA-Z0-9\-\.]+)$
    if (s.substr(pos, 1) != "/") return false;
    peertitle = s.substr(pos + 1);

    return find(ourtitle, peer, port, peertitle, series, security);
}
