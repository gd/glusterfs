/*
  Copyright (c) 2010-2011 Gluster, Inc. <http://www.gluster.com>
  This file is part of GlusterFS.

  GlusterFS is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; either version 3 of the License,
  or (at your option) any later version.

  GlusterFS is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see
  <http://www.gnu.org/licenses/>.
*/



#ifndef _CONFIG_H
#define _CONFIG_H
#include "config.h"
#endif

#include "rpcsvc.h"
#include "list.h"
#include "dict.h"
#include "xdr-rpc.h"


int
auth_unix_request_init (rpcsvc_request_t *req, void *priv)
{
        if (!req)
                return -1;
        memset (req->verf.authdata, 0, RPCSVC_MAX_AUTH_BYTES);
        req->verf.datalen = 0;
        req->verf.flavour = AUTH_NULL;

        return 0;
}

int auth_unix_authenticate (rpcsvc_request_t *req, void *priv)
{
        int                     ret = RPCSVC_AUTH_REJECT;
        struct authunix_parms   aup;
        char                    machname[MAX_MACHINE_NAME];

        if (!req)
                return ret;

        ret = xdr_to_auth_unix_cred (req->cred.authdata, req->cred.datalen,
                                     &aup, machname, req->auxgids);
        if (ret == -1) {
                gf_log ("", GF_LOG_WARNING, "failed to decode unix credentials");
                ret = RPCSVC_AUTH_REJECT;
                goto err;
        }

        req->uid = aup.aup_uid;
        req->gid = aup.aup_gid;
        req->auxgidcount = aup.aup_len;

        gf_log (GF_RPCSVC, GF_LOG_TRACE, "Auth Info: machine name: %s, uid: %d"
                ", gid: %d", machname, req->uid, req->gid);
        ret = RPCSVC_AUTH_ACCEPT;
err:
        return ret;
}

rpcsvc_auth_ops_t auth_unix_ops = {
        .transport_init              = NULL,
        .request_init           = auth_unix_request_init,
        .authenticate           = auth_unix_authenticate
};

rpcsvc_auth_t rpcsvc_auth_unix = {
        .authname       = "AUTH_UNIX",
        .authnum        = AUTH_UNIX,
        .authops        = &auth_unix_ops,
        .authprivate    = NULL
};


rpcsvc_auth_t *
rpcsvc_auth_unix_init (rpcsvc_t *svc, dict_t *options)
{
        return &rpcsvc_auth_unix;
}
