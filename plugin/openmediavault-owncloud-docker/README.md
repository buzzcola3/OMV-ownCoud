# openmediavault-owncloud-docker

Minimal OpenMediaVault plugin that deploys OwnCloud using the official container image via Podman. This is completely vibe coded — lightweight, focused, and meant to stay simple.

## License
Unlicensed / do-what-you-want. See the root `LICENSE` file.

## Features
- Stores settings in ConfDB (`conf.service.owncloud`).
- Workbench form to toggle enable, set image/tag, HTTP port, data directory, and optional admin credentials.
- mkconf script renders `/srv/owncloud/docker-compose.yml`.
- Salt state `omv.deploy.owncloud` calls mkconf and runs `podman-compose up -d` when enabled; stops the stack when disabled.

## Prerequisites
- OpenMediaVault 6/7 with Salt enabled.
- Podman (>= 4.x) and podman-compose installed on the host.
- `jq` available for mkconf JSON parsing.

## Building the package
From `plugin/openmediavault-owncloud-docker` run:

```
dpkg-buildpackage -b -us -uc
```

Install the generated `.deb` from the repository root (one level up).

## Installing from releases
1) Download the latest `.deb` asset from the GitHub Releases page (requires Podman + podman-compose on the host).
2) Copy it to your OMV box and install:
	- `sudo dpkg -i openmediavault-owncloud-docker_*.deb`
	- If dependencies are missing, run `sudo apt-get -f install`.
3) The plugin appears under Services → OwnCloud in the OMV UI.

### Continuous integration
- GitHub Actions workflow `build.yml` builds the Debian package on pushes/PRs.
- Artifacts: resulting `.deb` and build logs for inspection.

## Configuration fields
- `Enable OwnCloud`: whether the Salt state should bring the stack up.
- `Image` / `Tag`: Container image and tag (default `owncloud/server:10.14`).
- `HTTP port`: host port bound to container port 8080 (default 8088).
- `Data directory`: host path for persistent data (default `/srv/owncloud/data`).
- `Admin user` / `Admin password`: optional bootstrap credentials for the first start.

## Deployment flow
1. Save settings in the Workbench form; this writes ConfDB via the RPC service.
2. Apply changes triggers `omv-salt deploy run omv.deploy.owncloud`.
3. The Salt state runs `omv-mkconf owncloud` to regenerate the compose file and then `podman-compose up -d`.
4. If disabled, the Salt state runs `podman-compose down --remove-orphans` for cleanup.

## Notes
- HTTPS termination is assumed to be handled by a reverse proxy (e.g., Nginx or SWAG) in front of the HTTP port.
- SQLite is used by default. Extend the compose file manually if you need MariaDB/Redis.
- Data lives in the configured data directory; back it up before wiping or changing disks.

## Working config (Cloudflare tunnel + MariaDB)
- Keep `data/config/config.php` in your data directory; adjust it after the first start if you add a tunnel or DB.
- For MariaDB: set `dbtype => 'mysql'`, `dbhost => 'owncloud-db'`, `dbport => '3306'`, and ensure the image has the MySQL driver (official `owncloud/server:10.14` already does; otherwise install `php-mysql`).
- For Cloudflare tunnels: add your tunnel host (and host:port if non-443) to `trusted_domains`, then set `overwritehost` to that host and `overwriteprotocol` to `https` to avoid trusted-domain warnings.
- If a warning persists, check `data/files/owncloud.log` for the exact `host` value in the "Trusted domain error" entry and add that host string verbatim to `trusted_domains`.
- Data path: the plugin binds your selected data directory to `/mnt/data` inside the container. OwnCloud stores uploads under `/mnt/data/data/<username>/files/` on the host.
