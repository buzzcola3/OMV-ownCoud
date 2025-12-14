<?php

require_once "openmediavault/rpc/serviceabstract.inc";
require_once "openmediavault/config/database.inc";
require_once "openmediavault/system/process.inc";

class OMVRpcServiceOwnCloud extends \OMV\Rpc\ServiceAbstract
{
    public function getName()
    {
        return "OwnCloud";
    }

    public function initialize()
    {
        $this->registerMethod("getSettings");
        $this->registerMethod("setSettings");
        $this->registerMethod("applyChanges");
    }

    public function getSettings($params, $context)
    {
        $object = $this->getConfigObject("conf.service.owncloud");

        return $this->normalizeConfig($object);
    }

    public function setSettings($params, $context)
    {
        $object = $this->getConfigObject("conf.service.owncloud");
        if ($params instanceof \ArrayObject) {
            $params = $params->getArrayCopy();
        }

        $object->setAssoc((array) $params, true, true);

        $this->setConfigObject($object);

        return $this->normalizeConfig($object);
    }

    public function applyChanges($params, $context)
    {
        if (is_null($params)) {
            $params = array();
        }
        $this->validateMethodContext($context, array(
            "role" => OMV_ROLE_ADMINISTRATOR
        ));
        $this->validateMethodParams($params, "rpc.owncloud.applyChanges");

        // Run the deploy via Salt in the background so the UI can poll progress.
        return $this->execBgProc(function($bgStatusFilename, $bgOutputFilename) {
            $process = new \OMV\System\Process("omv-salt",
              array("deploy", "run", "owncloud"));
            $process->setRedirect2to1(true);
            if (0 !== ($exitStatus = $this->exec($process, $output,
                    $bgOutputFilename))) {
                throw new \OMV\ExecException($process, $output, $exitStatus);
            }
            return $output;
        });
    }

    private function getConfigObject($id)
    {
        $db = \OMV\Config\Database::getInstance();
        return $db->get($id);
    }

    private function setConfigObject(\OMV\Config\ConfigObject $object)
    {
        $db = \OMV\Config\Database::getInstance();
        $db->set($object);
    }

    private function normalizeConfig($object)
    {
        if ($object instanceof \OMV\Config\ConfigObject) {
            $object = $object->getAssoc();
        } elseif ($object instanceof \ArrayObject) {
            $object = $object->getArrayCopy();
        }

        $object['enable'] = filter_var($object['enable'] ?? false, FILTER_VALIDATE_BOOLEAN);
        $object['dataDir'] = $object['dataDir'] ?? '';

        return $object;
    }
}
