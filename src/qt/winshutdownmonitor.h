



#ifndef FREECOIN_QT_WINSHUTDOWNMONITOR_H
#define FREECOIN_QT_WINSHUTDOWNMONITOR_H

#ifdef WIN32
#include <QByteArray>
#include <QString>

#if QT_VERSION >= 0x050000
#include <windef.h> // for HWND

#include <QAbstractNativeEventFilter>

class WinShutdownMonitor : public QAbstractNativeEventFilter
{
public:
    /** Implements QAbstractNativeEventFilter interface for processing Windows messages */
    bool nativeEventFilter(const QByteArray &eventType, void *pMessage, long *pnResult);

    /** Register the reason for blocking shutdown on Windows to allow clean client exit */
    static void registerShutdownBlockReason(const QString& strReason, const HWND& mainWinId);
};
#endif
#endif

#endif // FREECOIN_QT_WINSHUTDOWNMONITOR_H
