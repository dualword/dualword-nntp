/*
    KNode, the KDE newsreader
    Copyright (c) 1999-2005 the KNode authors.
    See file AUTHORS for details

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, US
*/

#ifndef KNARTICLE_H
#define KNARTICLE_H

#include "knjobdata.h"

#ifndef Q_MOC_RUN
#include <boost/shared_ptr.hpp>
#endif
#include <QtCore/QBitArray>
#include <QFile>
#include <QColor>
#include <kmime/kmime_headers.h>
#include <kmime/kmime_newsarticle.h>

class KNLoadHelper;
class KNArticleCollection;

/** This class encapsulates a generic article. It provides all the
    usual headers of a RFC822-message. Further more it contains an
    unique id and can store a pointer to a @ref QListViewItem. It is
    used as a base class for all visible articles. */
class KNArticle : public KMime::NewsArticle, public KNJobItem {

  public:
    /// Shared pointer to a KNArticle. To be used instead of raw KNArticle*.
    typedef boost::shared_ptr<KNArticle> Ptr;
    /// List of articles.
    typedef QList<KNArticle::Ptr> List;

    enum articleType {
      ATremote,
      ATlocal
    };

    explicit KNArticle( boost::shared_ptr<KNArticleCollection> c );
    ~KNArticle();

    virtual void clear();

    virtual articleType type() const = 0;

    /** Returns the article id. */
    int id() const            { return i_d; }
    /** Sets the article id.
     * @param i The article id.
     */
    void setId( int i ) { i_d = i; }

    virtual bool filterResult() = 0;

    //network lock (reimplemented from KNJobItem)
    bool isLocked()                      { return f_lags.at(0); }
    void setLocked(bool b=true);

    //prevent that the article is unloaded automatically
    bool isNotUnloadable()               { return f_lags.at(1); }
    void setNotUnloadable(bool b=true)   { f_lags.setBit(1, b); }

    //article-collection
    boost::shared_ptr<KNArticleCollection> collection() const { return c_ol; }
    void setCollection( boost::shared_ptr<KNArticleCollection> c ) { c_ol = c; }
    bool isOrphant() const                           { return (i_d==-1); }

  protected:
    int i_d; //unique in the given collection
    boost::shared_ptr<KNArticleCollection> c_ol;

    QBitArray f_lags;

}; // KNArticle


class KNGroup;

/** KNRemoteArticle represents an article, whos body has to be
    retrieved from a remote host or from the local cache.
    All articles in a newsgroup are stored in instances
    of this class. */

class KNRemoteArticle : public KNArticle {

  public:
    /// Shared pointer to a KNRemoteArticle. To be used instead of raw KNRemoteArticle*.
    typedef boost::shared_ptr<KNRemoteArticle> Ptr;
    /// List of remote articles.
    typedef QList<KNRemoteArticle::Ptr> List;

    explicit KNRemoteArticle( boost::shared_ptr<KNGroup> g );
    ~KNRemoteArticle();

    // type
    virtual articleType type() const { return ATremote; }

    // content handling
    virtual void parse();

    // article number
    int articleNumber() const                 { return a_rticleNumber; }
    void setArticleNumber(int number)    { a_rticleNumber = number; }

    // status
    bool isNew()                         { return f_lags.at(2); }
    void setNew(bool b=true)             { f_lags.setBit(2, b); }
    bool getReadFlag()                   { return f_lags.at(3); }
    bool isRead()                        { return f_lags.at(7) || f_lags.at(3); }   // ignored articles == read
    void setRead(bool b=true)            { f_lags.setBit(3, b); }
    bool isExpired()                     { return f_lags.at(4); }
    void setExpired(bool b=true)         { f_lags.setBit(4, b); }
    bool isKept()                        { return f_lags.at(5); }
    void setKept(bool b=true)            { f_lags.setBit(5, b); }
    bool hasChanged()                    { return f_lags.at(6); }
    void setChanged(bool b=true)         { f_lags.setBit(6, b); }
    bool isIgnored()                     { return f_lags.at(7); }
    void setIgnored(bool b=true)         { f_lags.setBit(7, b); }
    bool isWatched()                     { return f_lags.at(8); }
    void setWatched(bool b=true)         { f_lags.setBit(8, b); }

    // thread info
    int idRef()                                     { return i_dRef; }
    void setIdRef(int i)                            { if (i != id())
                                                        i_dRef=i;
                                                      else
                                                        i_dRef=0; }
    KNRemoteArticle::Ptr displayedReference()           { return d_ref; }
    void setDisplayedReference( KNRemoteArticle::Ptr dr ) { d_ref=dr; }
    unsigned char threadingLevel()                { return t_hrLevel; }
    void setThreadingLevel(unsigned char l)       { t_hrLevel=l; }
    bool hasNewFollowUps()                        { return (n_ewFups>0); }
    void setNewFollowUps(unsigned short s)        { n_ewFups=s; }
    void incNewFollowUps(unsigned short s=1)      { n_ewFups+=s; }
    bool hasUnreadFollowUps()                     { return (u_nreadFups>0); }
    void setUnreadFollowUps(unsigned short s)     { u_nreadFups=s; }
    void incUnreadFollowUps(unsigned short s=1)   { u_nreadFups+=s; }

    //filtering
    bool filterResult()                     { return f_lags.at(10); }
    void setFilterResult(bool b=true)       { f_lags.setBit(10, b); }
    bool isFiltered()                       { return f_lags.at(11); }
    void setFiltered(bool b=true)           { f_lags.setBit(11, b); }
    bool hasVisibleFollowUps()              { return f_lags.at(12); }
    void setVisibleFollowUps(bool b=true)   { f_lags.setBit(12, b); }

    virtual void setForceDefaultCharset( bool b );

  protected:
    int a_rticleNumber;
    int i_dRef;                      // id of a reference-article (0 == none)
    KNRemoteArticle::Ptr d_ref;      // displayed reference-article (may differ from i_dRef)
    unsigned char t_hrLevel;         // quality of threading
    unsigned short u_nreadFups,      // number of the article's unread follow-ups
                   n_ewFups;         // number of the article's new follow-ups.

}; // KNRemoteArticle



/** This class encapsulates an article, that is
   stored locally in an MBOX-file. All own and
   saved articles are represented by instances
   of this class.
*/
class KNLocalArticle : public KNArticle {

  public:
    /// Shared pointer to a KNLocalArticle. To be used instead of raw KNLocalArticle*.
    typedef boost::shared_ptr<KNLocalArticle> Ptr;
    /// List of local articles.
    typedef QList<KNLocalArticle::Ptr> List;

    explicit KNLocalArticle( boost::shared_ptr<KNArticleCollection> c = boost::shared_ptr<KNArticleCollection>() );
    ~KNLocalArticle();

    //type
    virtual articleType type() const { return ATlocal; }

    //send article as mail
    bool doMail()                 { return f_lags.at(2); }
    void setDoMail(bool b=true)   { f_lags.setBit(2, b); }
    bool mailed()                 { return f_lags.at(3); }
    void setMailed(bool b=true)   { f_lags.setBit(3, b); }

    //post article to a newsgroup
    bool doPost()                 { return f_lags.at(4); }
    void setDoPost(bool b=true)   { f_lags.setBit(4, b); }
    bool posted()                 { return f_lags.at(5); }
    void setPosted(bool b=true)   { f_lags.setBit(5, b); }
    bool canceled()               { return f_lags.at(6); }
    void setCanceled(bool b=true) { f_lags.setBit(6, b); }

    // status
    bool pending()                { return ( (doPost() && !posted()) || (doMail() && !mailed()) ); }
    bool isSavedRemoteArticle()   {  return ( !doPost() && !doMail() && editDisabled() ); }

    //edit
    bool editDisabled()               { return f_lags.at(7); }
    void setEditDisabled(bool b=true) { f_lags.setBit(7, b); }

    //search
    bool filterResult()                { return f_lags.at(8); }
    void setFilterResult(bool b=true)  { f_lags.setBit(8, b); }

    //MBOX information
    int startOffset() const             { return s_Offset; }
    void setStartOffset(int so)   { s_Offset=so; }
    int endOffset() const              { return e_Offset; }
    void setEndOffset(int eo)     { e_Offset=eo; }

    //nntp-server id
    int serverId()                { if(!doPost()) return -1; else return s_erverId; }
    void setServerId(int i)       { s_erverId=i; }

    virtual void setForceDefaultCharset(bool b);

    protected:
      int s_Offset, //position in mbox-file : start
          e_Offset, //position in mbox-file : end
          s_erverId; //id of the nntp-server this article is posted to
};


/** KNAttachment represents a file that is
 *  or will be attached to an article.
 */
class KNAttachment {

  public:
    /**
      Shared pointer to a KNAttachment. To be used instead of raw KNAttachment*.
    */
    typedef boost::shared_ptr<KNAttachment> Ptr;

    explicit KNAttachment(KMime::Content *c);
    explicit KNAttachment(KNLoadHelper *helper);
    ~KNAttachment();

    //name (used as a Content-Type parameter and as filename)
    const QString& name()           { return n_ame; }
    void setName(const QString &s)  { n_ame=s; h_asChanged=true; }

    //mime type
    QString mimeType()            { return mMimeType; }
    void setMimeType(const QString &s);

    //Content-Description
    const QString& description()          { return d_escription; }
    void setDescription(const QString &s) { d_escription=s; h_asChanged=true; }

    //Encoding
    int cte()                             { return e_ncoding.encoding(); }
    void setCte(int e)                    { e_ncoding.setEncoding( (KMime::Headers::contentEncoding)(e) );
                                            h_asChanged=true; }
    bool isFixedBase64()const                  { return f_b64; }
    QString encoding()                    { return e_ncoding.asUnicodeString(); }

    //content handling
    KMime::Content* content()const             { return c_ontent; }
    QString contentSize() const;
    bool isAttached() const                    { return i_sAttached; }
    bool hasChanged() const                    { return h_asChanged; }
    void updateContentInfo();
    void attach(KMime::Content *c);
    void detach(KMime::Content *c);

  protected:
    KMime::Content *c_ontent;
    KNLoadHelper   *l_oadHelper;
    QFile *f_ile;
    QString mMimeType;
    QString n_ame,
            d_escription;
    KMime::Headers::ContentTransferEncoding e_ncoding;
    bool  i_sAttached,
          h_asChanged,
          f_b64;
};

Q_DECLARE_METATYPE(KNArticle::Ptr);
Q_DECLARE_METATYPE(KNRemoteArticle::Ptr);

#endif //KNARTICLE_H
