/** TSServerClientMgr.cc
 *
 *
 *
 *  Created on: Jul 31, 2014
 *      Author: Silvestro di Luise
 *              Silvestro.Di.Luise@cern.ch
 *
 *
 *
 *
 */

#include <iostream>
#include <assert.h>

#include "MessageMgr.h"

#include "ServerClientMgr.h"


using std::cout;
using std::endl;


bool ServerClientMgr::kBroadcastIsActive = false;

bool ServerClientMgr::kDebug = false;

ServerClientMgr::ServerClientMgr(TString owner_name) //,TSServerClientMgr& owner)
:fOwnerName(owner_name)
,kChanged(1)
,fOwner(this)
,kUnChanged(0),kRemoved(2)
,fAnyClientChanged(0),fAnyServerChanged(0)
,fNumOfClientsChanged(0),fNumOfServersChanged(0)
,fNumOfClients(0),fNumOfServers(0)
{
	// TODO Auto-generated constructor stub



}


ServerClientMgr::ServerClientMgr(const ServerClientMgr &other) {


	for(int i=1;i<=other.GetNumOfClients();++i){
		AddClient( GetClient(i) );
		SetClientStatus( GetClient(i),GetClientStatus(i) );
	}

	for(int i=1;i<=other.GetNumOfServers();++i){
		AddServer( GetServer(i) );
		SetServerStatus( GetServer(i),GetServerStatus(i) );
	}


}



ServerClientMgr::~ServerClientMgr() {
	// TODO Auto-generated destructor stub


	BroadcastToClients(kRemoved);
	BroadcastToServers(kRemoved);

}


void ServerClientMgr::AddClient(ServerClientMgr* o){

	if( !BroadcastActive() ) return;

	if( !o ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," receiving a null pointer ");
		return;
	}

	fNumOfClients++;
	fClients[fNumOfClients]=(ServerClientMgr*)o;

	SetClientStatus(o,kChanged);

	if(kDebug)
		MSG::LOG(__FILE__," adding client ",o->GetOwnerName()," to ",GetOwnerName());

}


void ServerClientMgr::AddServer(ServerClientMgr* o){

	if( !BroadcastActive() ) return;

	if( !o ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," receiving a null pointer ");
		return;
	}

	fNumOfServers++;
	fServers[fNumOfServers]=(ServerClientMgr*)o;

	SetServerStatus(o,kChanged);

	if(kDebug)
		MSG::LOG(__FILE__," adding server ",o->GetOwnerName()," to ",GetOwnerName());

}



void ServerClientMgr::BroadcastToClients(int status){

	//cout<<__FUNCTION__<<" "<<GetOwnerName()<<" status "<<status<<" num of clients: "<<GetNumOfClients()<<endl;

	if( !BroadcastActive() ) return;

	if( GetNumOfClients()==0 ) return;


	for(std::map<int,ServerClientMgr*>::iterator it= fClients.begin(), end = fClients.end();
			it!=end;
			++it){

		( (ServerClientMgr*)(it->second) )->SetServerStatus(fOwner,status);
	}



}


void ServerClientMgr::BroadcastToServers(int status){

	//cout<<__FUNCTION__<<" "<<GetOwnerName()<<" status "<<status<<" num of clients: "<<GetNumOfServers()<<endl;

	if( !BroadcastActive() ) return;

	if( GetNumOfServers()==0 ) return;

	for(std::map<int,ServerClientMgr*>::iterator it= fServers.begin(), end = fServers.end();
			it!=end;
			++it){
		( (ServerClientMgr*)(it->second) )->SetClientStatus(fOwner,status);
	}

}


ServerClientMgr* ServerClientMgr::GetClient(int n) const{

	if( n>GetNumOfClients() || fClients.count(n)==0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," index ",n," out of range");
		return 0;
	}

    fClients.find(n)->second;

}


int ServerClientMgr::GetClientStatus(int n) const{

	if( n>GetNumOfClients() || fClientsStatus.count(n)==0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," index ",n," out of range");
		return -1;
	}

    fClientsStatus.find(n)->second;

}


ServerClientMgr* ServerClientMgr::GetServer(int n) const {


	if( n>GetNumOfServers() || fServers.count(n)==0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," index ",n," out of range");
		return 0;
	}

	fServers.find(n)->second;

}


int ServerClientMgr::GetServerStatus(int n) const {


	if( n>GetNumOfServers() || fServersStatus.count(n)==0 ){
		MSG::ERROR(__FILE__,"::",__FUNCTION__," index ",n," out of range");
		return -1;
	}

	fServersStatus.find(n)->second;

}


TString ServerClientMgr::GetStatusName(int status) const{

	if(status==kChanged) return "changed";
	if(status==kUnChanged) return "un-changed";
	if(status==kRemoved) return "removed";

	return "unknown";
}


void ServerClientMgr::PrintServersAndClients(){

	cout<<endl;
	MSG::LOG("---",__FILE__,"::",__FUNCTION__,"---");

	cout<<" Owner: "<<GetOwnerName()
		<<" Num of Clients: "<<GetNumOfClients()<<" Num of Server: "<<GetNumOfServers()<<endl;

	if(GetNumOfClients()>0){

		cout<<" Clients List "<<endl;
		for(std::map<int,ServerClientMgr*>::iterator it= fClients.begin(), end = fClients.end();
				it!=end;
				++it){

				 ServerClientMgr *obj= (it->second);
				 int status = fClientsStatus[it->first];
				cout<<"  "<<it->first<<" "<<(TString)obj->GetOwnerName()<<" "<<obj<<" "<<GetStatusName(status)<<endl;
		}

		cout<<""<<endl;
	}


	if(GetNumOfServers()>0){

		cout<<" Servers List "<<endl;
		for(std::map<int,ServerClientMgr*>::iterator it= fServers.begin(), end = fServers.end();
				it!=end;
				++it){

			ServerClientMgr *obj= (it->second);
			int status= fServersStatus[it->first];
			cout<<"  "<<it->first<<" "<<obj->GetOwnerName()<<" "<<obj<<" "<<GetStatusName(status)<<endl;
		}

	}

	MSG::LOG("-----------");
}




void ServerClientMgr::RemoveClient(ServerClientMgr *o){

	if( !BroadcastActive() ) return;

	//workoround to keep the ordering sequential
	//must be changed for performance
	std::map<int,ServerClientMgr*> clients_list_new;
	std::map<int,int> clients_status_new;


	int N=0;

	for(std::map<int,ServerClientMgr*>::iterator it= fClients.begin(), end = fClients.end();
				it!=end;
				++it){

			    ServerClientMgr *ob=it->second;

			    int status = fClientsStatus[it->first];

			    if(ob==o){
			    	//fClients.erase(it);
			    	//fClientsStatus.erase(it->first);
			    	fNumOfClients--;
			    	continue;
			    }

		N++;
		clients_list_new[N]=ob;
		clients_status_new[N]=status;

	}

	fClients.clear();
	fClientsStatus.clear();

	fClients.insert(clients_list_new.begin(),clients_list_new.end());
	fClientsStatus.insert(clients_status_new.begin(),clients_status_new.end());


}


void ServerClientMgr::RemoveServer(ServerClientMgr *o){

	if( !BroadcastActive() ) return;

	//workoround to keep the ordering sequential
	//must be changed for performance
	std::map<int,ServerClientMgr*> servers_list_new;
	std::map<int,int> servers_status_new;

	int N=0;

	for(std::map<int,ServerClientMgr*>::iterator it= fServers.begin(), end = fServers.end();
				it!=end;
				++it){

				ServerClientMgr *ob=it->second;

				int status = fClientsStatus[it->first];

			    if(ob==o){
			    	//fServers.erase(it);
			    	//fServersStatus.erase(it->first);
			    	fNumOfServers--;
			    	return;
			    }

			    N++;
			    servers_list_new[N]=ob;
			    servers_status_new[N]=status;
	}

	fServers.clear();
	fServersStatus.clear();

	fServers.insert(servers_list_new.begin(),servers_list_new.end());
	fServersStatus.insert(servers_status_new.begin(),servers_status_new.end());

}



void ServerClientMgr::ResetAll(){

	fClients.clear();
	fClientsStatus.clear();
	fServers.clear();
	fServersStatus.clear();

	fResetCounters();

}

void ServerClientMgr::fResetCounters(){

	fAnyClientChanged=0;
	fAnyServerChanged=0;
	fNumOfClientsChanged=0;
	fNumOfServersChanged=0;
	fNumOfClients=0;
	fNumOfServers=0;

}


bool ServerClientMgr::ServerChanged(ServerClientMgr *server){

	if( !BroadcastActive() ) return false;

	for(std::map<int,ServerClientMgr*>::iterator it= fServers.begin(), end = fServers.end();
			it!=end;
			++it){

			ServerClientMgr *s=it->second;

		    if(s==server){
		    	if( fServersStatus[it->first] == kChanged ){
		    		return true;
		    	}else{
		    		return false;
		    	}

		    }
	}

	return false;

}


bool ServerClientMgr::ServerRemoved(ServerClientMgr *server){

	if( !BroadcastActive() ) return false;

	for(std::map<int,ServerClientMgr*>::iterator it= fServers.begin(), end = fServers.end();
			it!=end;
			++it){

			ServerClientMgr *s=it->second;

		    if(s==server){
		    	if( fServersStatus[it->first] == kChanged ){
		    		return true;
		    	}else{
		    		return false;
		    	}

		    }
	}

	return false;

}


void ServerClientMgr::SetClientStatus(ServerClientMgr *client, int status){

	if( !BroadcastActive() ) return;

	if(kDebug)
		MSG::LOG("TSServerClientMgr::SetClientStatus ",client->GetOwnerName()," of ",GetOwnerName()," set to: ",GetStatusName(status));

	fNumOfClientsChanged =0;

	for(std::map<int,ServerClientMgr*>::iterator it= fClients.begin(), end = fClients.end();
			it!=end;
			++it){

			ServerClientMgr *c=it->second;

		    if(c==client){

                //if(status==kRemoved){
                //	RemoveClient(client);
                //}else{
                //	fClientsStatus[it->first]=status;
                //}

		    	fClientsStatus[it->first]=status;
		    }

		    if( fClientsStatus[it->first]==kChanged || fClientsStatus[it->first]==kRemoved ){
		    	fNumOfClientsChanged++;
		    }

	}


	if(status==kRemoved){
		RemoveClient(client);
	}

	fNumOfClientsChanged>0 ? fAnyClientChanged = true: fAnyClientChanged = false;

	//if(fNumOfClientsChanged>0) fAnyClientChanged = true;
	//else fAnyClientChanged = false;

}




void ServerClientMgr::SetServerStatus(ServerClientMgr *server, int status){


	if( !BroadcastActive() ) return;

	if(kDebug)
		MSG::LOG("TSServerClientMgr::SetServerStatus ",server->GetOwnerName()," of ",GetOwnerName()," set to: ",GetStatusName(status));

	fNumOfServersChanged =0;

	for(std::map<int,ServerClientMgr*>::iterator it= fServers.begin(), end = fServers.end();
			it!=end;
			++it){

			ServerClientMgr *s=it->second;

		    if(s==server){

		    	//if(status==kRemoved){
		    	//	RemoveServer(server);
		    	//}else{
		    	//	fServersStatus[it->first]=status;
		    	//}

		    	fServersStatus[it->first]=status;
		    }


		    if( fServersStatus[it->first]==kChanged || fServersStatus[it->first]==kRemoved ){
		    	fNumOfServersChanged++;
		    }
	}


	if(status==kRemoved){
		RemoveServer(server);
	}


	fNumOfServersChanged>0 ? fAnyServerChanged = true: fAnyServerChanged = false;

	//if(fNumOfServersChanged>0) fAnyServerChanged = true;
	//else fAnyServerChanged = false;

}
