#ifndef TORRENTEXPORTER_H
#define TORRENTEXPORTER_H

#include <QObject>
#include <QString>
#include <QPointer>
#include <QHash>
#include <QSet>
#include <QMutex>

class P2PNetwork;
class TorrentDatabase;
class QWidget;
struct TorrentInfo;

/**
 * @brief TorrentExporter - exports torrents as .torrent files
 *
 * Caches generated .torrent files in {dataDirectory}/torrents/{hash}.torrent.
 * If a cached copy is available it is offered for save immediately; otherwise
 * metadata is fetched (BEP 9, no full download), saved into the cache, and
 * then offered to the user.
 */
class TorrentExporter : public QObject
{
    Q_OBJECT

public:
    explicit TorrentExporter(QObject *parent = nullptr);
    ~TorrentExporter();

    void setP2PNetwork(P2PNetwork *network) { p2pNetwork_ = network; }
    void setDatabase(TorrentDatabase *db) { database_ = db; }
    void setDataDirectory(const QString &dir);

    /**
     * @brief Export a torrent to a .torrent file
     * Shows the user a progress message in @p parent's status bar (when
     * possible), fetches metadata if needed, then prompts for a save location.
     */
    void exportTorrent(QWidget *parent, const TorrentInfo &torrent);

signals:
    void exportFailed(const QString &hash, const QString &reason);
    void exportSucceeded(const QString &hash, const QString &savedPath);
    void statusMessage(const QString &message, int timeoutMs);

private:
    QString cacheFilePath(const QString &hash) const;
    QString suggestedFileName(const TorrentInfo &torrent) const;
    bool ensureCacheDir() const;

    void fetchMetadataAndSave(QWidget *parent, const TorrentInfo &torrent);
    void promptAndCopy(QWidget *parent, const QString &cachePath, const TorrentInfo &torrent);

    P2PNetwork *p2pNetwork_ = nullptr;
    TorrentDatabase *database_ = nullptr;
    QString dataDirectory_;

    // In-flight metadata fetches (avoid duplicate concurrent requests for same hash)
    QMutex inFlightMutex_;
    QSet<QString> inFlight_;
};

#endif // TORRENTEXPORTER_H
