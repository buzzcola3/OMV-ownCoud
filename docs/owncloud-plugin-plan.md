# OwnCloud OMV plugin plan

Assumptions and scope:
- Deploy OwnCloud via a single Docker container using the official `owncloud/server` image.
- Allow configuring enable/disable toggle, container image/tag, host ports, data directory, and optional admin credentials (for first start).
- Use Salt to render and apply a minimal docker-compose stack in `/srv/owncloud/docker-compose.yml`.
- Persist data under the configured data directory (default `/srv/owncloud/data`).
- Expose HTTP port only (HTTPS assumed to be handled by a reverse proxy in OMV or elsewhere).
- Keep database simple by using the built-in SQLite for now; users can later extend the compose template for external DB if needed.

Planned plugin pieces:
- ConfDB defaults and datamodel for `conf.service.owncloud`.
- RPC service (PHP) to get/set settings and trigger apply.
- mkconf script to generate `docker-compose.yml` from ConfDB values.
- Salt state `omv.deploy.owncloud` to call mkconf and `docker compose up -d`.
- Workbench component descriptor to surface the settings in OMV UI.

Out of scope for v1:
- TLS termination inside container (assume upstream reverse proxy).
- External database/redis wiring.
- Backup/restore automation.
