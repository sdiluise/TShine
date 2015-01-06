/*
 * TSServerClientMgr.h
 *
 *  Created on: Jul 31, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 *
 */

#ifndef TSSERVERCLIENTMGR_H_
#define TSSERVERCLIENTMGR_H_

#include <map>

#include <TString.h>


class ServerClientMgr {

public:

	ServerClientMgr(TString owner_name);//,TSServerClientMgr& owner);
	ServerClientMgr(const ServerClientMgr &);
	virtual ~ServerClientMgr();

	void AddClient(ServerClientMgr *);
    void AddServer(ServerClientMgr *);

    bool AnyClientChanged() {return fAnyClientChanged;}
    bool AnyServerChanged() {return fAnyServerChanged;}

    bool BroadcastActive() {return kBroadcastIsActive;}
    void BroadcastToClients(int);
    void BroadcastToServers(int);

    ServerClientMgr* GetClient(int) const;
    int GetClientStatus(int) const;
    ServerClientMgr* GetServer(int) const;
    int GetServerStatus(int) const;
    int GetNumOfClients() const {return fNumOfClients;}
    int GetNumOfServers() const {return fNumOfServers;}
    int GetNumOfClientsChanged() const {return fNumOfClientsChanged;}
    int GetNumOfServersChanged() const {return fNumOfServersChanged;}

    TString GetOwnerName() const {return fOwnerName;}
    TString GetStatusName(int) const;

    void PrintServersAndClients();
    void RemoveClient(ServerClientMgr*);
    void RemoveServer(ServerClientMgr*);
    void ResetAll();
    void SetClientStatus(ServerClientMgr *, int);
    void SetServerStatus(ServerClientMgr *, int);

    bool ServerChanged(ServerClientMgr *);
    bool ServerRemoved(ServerClientMgr *);

    int kChanged;
    int kUnChanged;
    int kRemoved;

    static bool kBroadcastIsActive;
    static bool kDebug;

private:

    TString fOwnerName;

    ServerClientMgr *fOwner;

    bool fAnyClientChanged;
    bool fAnyServerChanged;
    int fNumOfClientsChanged;
    int fNumOfServersChanged;

    int fNumOfClients;
    int fNumOfServers;
	std::map<int,ServerClientMgr*> fServers;
	std::map<int,ServerClientMgr*> fClients;
	std::map<int,int> fClientsStatus;
	std::map<int,int> fServersStatus;

	void fResetCounters();
};

#endif /* TSSERVERCLIENTMGR_H_ */
