{%- set cfg = salt['omv_conf.get']('conf.service.owncloud') -%}
{%- set enabled = cfg.get('enable', False) | to_bool -%}

{# Render compose file and manage container based on enable flag #}
{%- if enabled %}
owncloud-mkconf:
  cmd.run:
    - name: /usr/share/openmediavault/mkconf/owncloud

owncloud-compose-up:
  cmd.run:
    - name: cd /srv/owncloud && docker-compose up -d
    - require:
      - cmd: owncloud-mkconf
{%- else %}
owncloud-compose-down:
  cmd.run:
    - name: test -f /srv/owncloud/docker-compose.yml && cd /srv/owncloud && docker-compose down --remove-orphans || true
{%- endif %}
