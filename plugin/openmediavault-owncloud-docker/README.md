# openmediavault-owncloud-docker

Minimal OpenMediaVault plugin that deploys OwnCloud using the official Docker image. This is completely vibe coded — lightweight, focused, and meant to stay simple.

## License
Unlicensed / do-what-you-want. See the root `LICENSE` file.

## Features
- Stores settings in ConfDB (`conf.service.owncloud`).
- Workbench form to toggle enable, set image/tag, HTTP port, data directory, and optional admin credentials.
- mkconf script renders `/srv/owncloud/docker-compose.yml`.
- Salt state `omv.deploy.owncloud` calls mkconf and runs `docker compose up -d` when enabled; stops the stack when disabled.

## Prerequisites
- OpenMediaVault 6/7 with Salt enabled.
- Docker Engine and Docker Compose installed on the host.
- `jq` available for mkconf JSON parsing.

## Building the package
From `plugin/openmediavault-owncloud-docker` run:

```
dpkg-buildpackage -b -us -uc
```

Install the generated `.deb` from the repository root (one level up).

### Continuous integration
- GitHub Actions workflow `build.yml` builds the Debian package on pushes/PRs.
- Artifacts: resulting `.deb` and build logs for inspection.

## Configuration fields
- `Enable OwnCloud`: whether the Salt state should bring the stack up.
- `Image` / `Tag`: Docker image and tag (default `owncloud/server:10.14`).
- `HTTP port`: host port bound to container port 8080.
- `Data directory`: host path for persistent data (default `/srv/owncloud/data`).
- `Admin user` / `Admin password`: optional bootstrap credentials for the first start.

## Deployment flow
1. Save settings in the Workbench form; this writes ConfDB via the RPC service.
2. Apply changes triggers `omv-salt deploy run omv.deploy.owncloud`.
3. The Salt state runs `omv-mkconf owncloud` to regenerate the compose file and then `docker compose up -d`.
4. If disabled, the Salt state runs `docker compose down --remove-orphans` for cleanup.

## Notes
- HTTPS termination is assumed to be handled by a reverse proxy (e.g., Nginx or SWAG) in front of the HTTP port.
- SQLite is used by default. Extend the compose file manually if you need MariaDB/Redis.
- Data lives in the configured data directory; back it up before wiping or changing disks.
