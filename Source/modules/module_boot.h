#pragma once

#include "modules/module.h"
#include "entity/entity_parser.h"

class CModuleBoot : public IModule
{
  std::vector< TEntityParseContext > ctxs;
  void loadScene(const std::string& strfilename);
public:
  CModuleBoot(const std::string& name) : IModule(name) { }
  void onFileChanged(const std::string& strfilename);
  bool start() override;
};

