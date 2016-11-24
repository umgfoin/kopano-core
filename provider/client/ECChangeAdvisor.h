/*
 * Copyright 2005 - 2016 Zarafa and its licensors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License, version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ECCHANGEADVISOR_H
#define ECCHANGEADVISOR_H

#include <kopano/zcdefs.h>
#include <mutex>
#include <mapidefs.h>
#include <mapispi.h>
#include <kopano/ECUnknown.h>
#include <IECChangeAdvisor.h>
#include "ics_client.hpp"
#include <kopano/kcodes.h>

#include <map>

namespace KC {
class ECLogger;
}

class ECMsgStore;

/**
 * ECChangeAdvisor: Implementation IECChangeAdvisor, which allows one to register for 
 *                  change notifications on folders.
 */
class ECChangeAdvisor _kc_final : public ECUnknown {
protected:
	/**
	 * Construct the ChangeAdvisor.
	 *
	 * @param[in]	lpMsgStore
	 *					The message store that contains the folder to be registered for
	 *					change notifications.
	 */
	ECChangeAdvisor(ECMsgStore *lpMsgStore);

	/**
	 * Destructor.
	 */
	virtual ~ECChangeAdvisor();

public:
	/**
	 * Construct the ChangeAdvisor.
	 *
	 * @param[in]	lpMsgStore
	 *					The message store that contains the folder to be registered for
	 *					change notifications.
	 * @param[out]	lppChangeAdvisor
	 *					The new change advisor.
	 */
	static	HRESULT Create(ECMsgStore *lpMsgStore, ECChangeAdvisor **lppChangeAdvisor);

	/**
	 * Get an alternate interface to the same object.
	 *
	 * @param[in]	refiid
	 *					A reference to the IID of the requested interface.
	 *					Supported interfaces:
	 *						- IID_ECChangeAdvisor
	 *						- IID_IECChangeAdvisor
	 * @param[out]	lpvoid
	 *					Pointer to a pointer of the requested type, casted to a void pointer.
	 */
	virtual HRESULT QueryInterface(REFIID refiid, void **lpvoid);

	// IECChangeAdvisor
	virtual HRESULT GetLastError(HRESULT hResult, ULONG ulFlags, LPMAPIERROR *lppMAPIError);
	virtual HRESULT Config(LPSTREAM lpStream, LPGUID lpGUID, IECChangeAdviseSink *lpAdviseSink, ULONG ulFlags);
	virtual HRESULT UpdateState(LPSTREAM lpStream);
	virtual HRESULT AddKeys(LPENTRYLIST lpEntryList);
	virtual HRESULT RemoveKeys(LPENTRYLIST lpEntryList);
	virtual HRESULT IsMonitoringSyncId(syncid_t ulSyncId);
	virtual HRESULT UpdateSyncState(syncid_t ulSyncId, changeid_t ulChangeId);

private:
	typedef std::map<syncid_t, connection_t> ConnectionMap;
	typedef std::map<syncid_t, changeid_t> SyncStateMap;

	/**
	 * Get the sync id from a ConnectionMap entry.
	 *
	 * @param[in]	sConnection
	 *					The ConnectionMap entry from which to extract the sync id.
	 * @return The sync id extracted from the the ConnectionMap entry.
	 */
	static ULONG					GetSyncId(const ConnectionMap::value_type &sConnection);

	/**
	 * Create a SyncStateMap entry from an SSyncState structure.
	 *
	 * @param[in]	sSyncState
	 *					The SSyncState structure to be converted to a SyncStateMap entry.
	 * @return A new SyncStateMap entry.
	 */
	static SyncStateMap::value_type	ConvertSyncState(const SSyncState &sSyncState);

	static SSyncState				ConvertSyncStateMapEntry(const SyncStateMap::value_type &sMapEntry);

	/**
	 * Compare the sync ids from a ConnectionMap entry and a SyncStateMap entry.
	 *
	 * @param[in]	sConnection
	 *					The ConnectionMap entry to compare the sync id with.
	 * @param[in]	sSyncState
	 *					The SyncStateMap entry to compare the sync id with.
	 * @return true if the sync ids are equal, false otherwise.
	 */
	static bool						CompareSyncId(const ConnectionMap::value_type &sConnection, const SyncStateMap::value_type &sSyncState);

	/**
	 * Reload the change notifications.
	 *
	 * @param[in]	lpParam
	 *					The parameter passed to AddSessionReloadCallback.
	 * @param[in]	newSessionId
	 *					The sessionid of the new session.
	 */
	static HRESULT					Reload(void *lpParam, ECSESSIONID newSessionId);

	/**
	 * Purge all unused connections from advisor.
	 */
	HRESULT							PurgeStates();

	class xECChangeAdvisor _kc_final : public IECChangeAdvisor {
		#include <kopano/xclsfrag/IUnknown.hpp>

		// <kopano/xclsfrag/IECChangeAdvisor.hpp>
		virtual HRESULT __stdcall GetLastError(HRESULT hResult, ULONG flags, LPMAPIERROR *lppMAPIError) _kc_override;
		virtual HRESULT __stdcall Config(LPSTREAM lpStream, LPGUID lpGUID, IECChangeAdviseSink *lpAdviseSink, ULONG flags) _kc_override;
		virtual HRESULT __stdcall UpdateState(LPSTREAM lpStream) _kc_override;
		virtual HRESULT __stdcall AddKeys(LPENTRYLIST lpEntryList) _kc_override;
		virtual HRESULT __stdcall RemoveKeys(LPENTRYLIST lpEntryList) _kc_override;
		virtual HRESULT __stdcall IsMonitoringSyncId(ULONG ulSyncId) _kc_override;
		virtual HRESULT __stdcall UpdateSyncState(ULONG ulSyncId, ULONG ulChangeId) _kc_override;
	} m_xECChangeAdvisor;

	ECMsgStore				*m_lpMsgStore;
	IECChangeAdviseSink *m_lpChangeAdviseSink;
	ULONG					m_ulFlags;
	std::recursive_mutex m_hConnectionLock;
	ConnectionMap			m_mapConnections;
	SyncStateMap			m_mapSyncStates;
	ECLogger				*m_lpLogger;
	ULONG					m_ulReloadId;
};

#endif // ndef ECCHANGEADVISOR_H
