#include "mcv_platform.h"
#include "module_boot.h"

bool CModuleBoot::start()
{
  std::string boot_name("module_boot");
  json json = loadJson("data/boot.json");
  auto prefabs = json["scenes_to_load"].get< std::vector< std::string > >();
  for (auto& p : prefabs)
    loadScene(p);
  return true;
}

void CModuleBoot::loadScene(const std::string& p) {
  PROFILE_FUNCTION("loadScene");
  TEntityParseContext ctx;
  dbg("Parsing %s\n", p.c_str());
  parseScene(p, ctx);
  ctxs.push_back(ctx);
}

void CModuleBoot::onFileChanged(const std::string& strfilename) {
  int idx = 0;
  for (auto& ctx : ctxs) {

    if (ctx.filename == strfilename) {
      // Destroy previous entities
      for (auto h : ctx.all_entities_loaded)
        h.destroy();

      ctxs.erase(ctxs.begin() + idx);

      // and reload the file
      loadScene(strfilename);

      CHandleManager::destroyAllPendingObjects();
      return;
    }

    ++idx;
  }
}
