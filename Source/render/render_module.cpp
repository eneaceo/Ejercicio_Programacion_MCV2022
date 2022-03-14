#pragma once

#include "mcv_platform.h"
#include "engine.h"
#include "render/render_module.h"
#include "render/draw_primitives.h"
#include "geometry/geometry.h"
#include "geometry/camera.h"
#include "render/shaders/pipeline_state.h"
#include "render/compute/compute_shader.h"
#include "render/render_manager.h"
#include "components/common/comp_light_spot.h"
#include "components/common/comp_particles.h"
#include "components/postfx/comp_color_grading.h"
#include "components/postfx/comp_blur.h"
#include "components/postfx/comp_focus.h"
#include "components/postfx/comp_edge_detector.h"
#include "components/postfx/comp_bloom.h"
#include "skeleton/comp_skeleton.h"

#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/ImGuizmo.h"

/// --------------------------- TO BE MOVED 
CShaderCte<CtesCamera> cte_camera;
CShaderCte<CtesObject> cte_object;
CShaderCte<CtesLight> cte_light;
CShaderCte<CtesWorld> cte_world;
CShaderCte<CtesUI> cte_ui;
CShaderCte<CtesParticle> cte_particle;
CShaderCte<CtesBlur> cte_blur;

/// -----------------------------------
static NamedValues<int>::TEntry output_entries[] = {
  {OUTPUT_COMPLETE, "Complete"},
  {OUTPUT_ALBEDOS, "Albedo"},
  {OUTPUT_NORMALS, "Normals"},
  {OUTPUT_SCREEN_SPACE_AO, "ScreenSpace AO"},
  {OUTPUT_BEFORE_TONE_MAPPING, "Before Tone Mapping"},
  {OUTPUT_BEFORE_GAMMA_CORRECTION, "Before Gamma Correction"},
};
static NamedValues<int> output_names(output_entries, sizeof(output_entries) / sizeof(NamedValues<int>::TEntry));

/// -----------------------------------

static void ImGuiCreate() {
  PROFILE_FUNCTION("ImGuiCreate");
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  //ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  //ImGui::StyleColorsDark();
  ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplWin32_Init(CApplication::get().getHandle());
  ImGui_ImplDX11_Init(Render.device, Render.ctx);
}

static void ImGuiDestroy() {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

static void ImGuiBeginFrame() {
  PROFILE_FUNCTION("ImGui");
  // Start the Dear ImGui frame
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  ImGuizmo::BeginFrame();
}

static void ImGuiEndFrame() {
  PROFILE_FUNCTION("Imgui");
  CGpuScope gpu_scope("Imgui");
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool CRenderModule::start() {
  ImGuiCreate();

  setClearColor({ 0.0f, 0.125f, 0.3f, 1.0f }); // red,green,blue,alpha

  createAndRegisterPrimitives();

  loadPipelines();
  loadComputeShaders();

  cte_object.create(CB_SLOT_OBJECT, "Object");
  cte_object.object_world = MAT44::Identity;
  cte_camera.create(CB_SLOT_CAMERA, "Camera");
  cte_light.create(CB_SLOT_LIGHT, "Light");
  cte_ui.create(CB_SLOT_UI, "UI");
  cte_particle.create(CB_SLOT_PARTICLES, "Particle");
  cte_blur.create(CB_SLOT_BLUR, "Blur");

  cte_world.create(CB_SLOT_WORLD, "World");
  cte_world.ambient_factor = 0.3f;
  cte_world.exposure_factor = 1.0f;
  cte_world.irrandiance_texture_mipmap_factor = 0.0f;
  cte_world.output_channel = OUTPUT_COMPLETE;
  cte_world.amount_color_grading = 0.0f;

  bool is_ok = true;
  is_ok &= deferred_renderer.create(Render.getWidth(), Render.getHeight());
  assert(is_ok);

  rt_deferred_output = new CRenderToTexture;
  is_ok = rt_deferred_output->createRT("rt_deferred_output.dds", Render.getWidth(), Render.getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, true);

  rt_final = new CRenderToTexture;
  is_ok = rt_final->createRT("rt_final.dds", Render.getWidth(), Render.getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, true);

  return true;
}


void CRenderModule::createPipelineFromJson(const std::string& name, const json& jdef) {
  TFileContext file_context(name);
  PROFILE_FUNCTION(name);
  const CResourceType* pipeline_type = getClassResourceType<CPipelineState>();
  // Check if we need to create a new pipeline object or we are rereading an existing one
  CPipelineState* pipeline = nullptr;
  auto it = my_pipelines.find(name);
  if (it == my_pipelines.end()) {
    // Not found... create a new one
    pipeline = new CPipelineState();
    my_pipelines[name] = pipeline;
    Resources.registerResource(pipeline, name, pipeline_type);
  }
  else
    pipeline = it->second;

  // Let the pipeline reconfigure itself from the json definition
  pipeline->create(jdef);
}

void CRenderModule::loadPipelines() {
  PROFILE_FUNCTION("loadPipelines");
  // This needs to go somewhere else
  json j = loadJson("data/pipelines.json");

  const CResourceType* pipeline_type = getClassResourceType<CPipelineState>();

  // Register all resource types defined in the json
  for (auto it : j.items()) {
    const std::string& key = it.key();
    const json& jdef = it.value();
    std::string name = key + pipeline_type->getExtension();

    createPipelineFromJson(name, jdef);

    // Once the pipeline has been configured....
    if (jdef.value("add_skin_support", false)) {
      json jdef_new = jdef;
      std::string vs_entry = jdef.value("vs_entry", "VS");
      jdef_new["vs_entry"] = vs_entry + "_Skin";
      std::string vs_decl = jdef["vertex_decl"];
      jdef_new["vertex_decl"] = vs_decl + "Skin";
      std::string new_name = key + "_skin" + pipeline_type->getExtension();
      jdef_new.erase("add_skin_support");
      jdef_new.erase("add_instanced_support");
      jdef_new["use_skin"] = true;
      createPipelineFromJson(new_name, jdef_new);
    }

    // Once the pipeline has been configured....
    if (jdef.value("add_instanced_support", false)) {
      json jdef_new = jdef;
      std::string vs_entry = jdef.value("vs_entry", "VS");
      jdef_new["vs_entry"] = vs_entry + "_Instanced";
      std::string new_name = key + "_instanced" + pipeline_type->getExtension();
      jdef_new.erase("add_skin_support");
      jdef_new.erase("add_instanced_support");
      jdef_new["use_instanced"] = true;
      createPipelineFromJson(new_name, jdef_new);
    }

  }

}


void CRenderModule::loadComputeShaders() {
  PROFILE_FUNCTION("loadCompute");
  // This needs to go somewhere else
  json j = loadJson("data/computes.json");

  const CResourceType* compute_type = getClassResourceType<CComputeShader>();
  // Register all resource types defined in the json
  for (auto it : j.items()) {
    const std::string& key = it.key();
    const json& jdef = it.value();
    std::string name = key + compute_type->getExtension();

    TFileContext file_context(name);
    PROFILE_FUNCTION(name);
    // Check if we need to create a new pipeline object or we are rereading an existing one
    CComputeShader* compute = new CComputeShader();
    compute->create(jdef);
    Resources.registerResource(compute, name, compute_type);
  }
}

void CRenderModule::onFileChanged(const std::string& filename) {
  if (filename == "data/pipelines.json")
    loadPipelines();

  // Reload pipelines
  if (filename.find("data/shaders/") != std::string::npos)
    loadPipelines();
}

// This is the module render painting something....
void CRenderModule::render() {
  PROFILE_FUNCTION("render");
  float ClearColor[4] = { _clearColor.x, _clearColor.y, _clearColor.z, _clearColor.w }; // red,green,blue,alpha
  //Render.ctx->ClearRenderTargetView(Render.render_target_view, ClearColor);
  //Render.ctx->ClearDepthStencilView(Render.depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  //// Hack temporal... activate all comp light spot, only the last will survive.e...
  //getObjectManager<TCompLightSpot>()->forEach([](TCompLightSpot* light_spot) {
  //  light_spot->activate();
  //  });

  //RenderManager.renderAll(eRenderChannel::RC_SOLID);
}

// This is the module render painting something....
void CRenderModule::renderInMenu() {
  RenderManager.renderInMenu();
  Resources.renderInMenu();
  ImGui::Text("Time: %lf (Delta:%f FPS:%f)", Time.current, Time.delta_unscaled, 1.0f / Time.delta_unscaled);
  ImGui::DragFloat("Time Factor", &Time.scale_factor, 0.01f, 0.f, 1.0f);

  ImGui::DragFloat("Ambient Factor", &cte_world.ambient_factor, 0.01f, 0.f, 1.0f);
  ImGui::DragFloat("Exposure Factor", &cte_world.exposure_factor, 0.01f, 0.f, 5.0f);
  ImGui::DragFloat("Irradiance Mode Factor", &cte_world.irrandiance_texture_mipmap_factor, 0.01f, 0.f, 1.0f);

  output_names.debugInMenu("Output", cte_world.output_channel);

  static int nframes = 3;
  ImGui::DragInt("NumFrames To Capture", &nframes, 0.1f, 1, 20);
  if (ImGui::SmallButton("Start CPU Trace...")) {
    PROFILE_SET_NFRAMES(nframes);
  }
}

void CRenderModule::activateMainCamera() {
  CEntity* e_camera = CEngine::get().getRender().getActiveCamera();
  if (!e_camera) {
    e_camera = getEntityByName("camera");
  }
  if (e_camera) {
    assert(e_camera);
    TCompCamera* active_camera = e_camera->get<TCompCamera>();
    assert(active_camera);
    active_camera->setAspectRatio((float)Render.getWidth() / (float)Render.getHeight());
    activateCamera(*active_camera);

    TCompColorGrading* c_color_grading = e_camera->get<TCompColorGrading>();
    c_color_grading->lut->activate(TS_LUT_COLOR_GRADING);
    cte_world.amount_color_grading = c_color_grading->getAmount();
  }
}

void CRenderModule::updateWorldCtes() {
  cte_world.render_width = (float)Render.getWidth();
  cte_world.render_height = (float)Render.getHeight();
  cte_world.inv_render_width = 1.0f / cte_world.render_width;
  cte_world.inv_render_height = 1.0f / cte_world.render_height;
  cte_world.world_time = Time.current;
  cte_world.world_delta_time = Time.delta;
  cte_world.updateFromCPU();
}

void CRenderModule::updateAllCtesBones() {
	// This will activate all the lights, but right now, only the last one will stay in the GPU
	getObjectManager<TCompSkeleton>()->forEach([](TCompSkeleton * c) {
		c->updateCteBones();
	});
}

void CRenderModule::generateShadowMaps() {
  CGpuScope gpu_scope("shadowMaps");
  getObjectManager<TCompLightSpot>()->forEach([](TCompLightSpot* light_spot) {
    light_spot->generateShadowMap();
    });
}

void CRenderModule::generateFrame() {
  PROFILE_FUNCTION("generateFrame");
  ImGuiBeginFrame();

  // This needs to go somewhere else
  cte_camera.activate();
  cte_object.activate();
  cte_light.activate();
  cte_world.activate();
  cte_ui.activate();

  updateWorldCtes();

  updateAllCtesBones();

  generateShadowMaps();

  activateMainCamera();

  CTexture::deactivate(TS_DEFERRED_OUTPUT);
  CHandle h_camera = CEngine::get().getRender().getActiveCamera();
  deferred_renderer.render(rt_deferred_output, h_camera);

  rt_final->activateRT();
  rt_deferred_output->activate(TS_DEFERRED_OUTPUT);
  drawFullScreenQuad("deferred_resolve.pipeline", rt_deferred_output);

  RenderManager.renderAll(eRenderChannel::RC_TRANSPARENT, h_camera);

  RenderManager.renderAll(eRenderChannel::RC_DISTORSIONS, h_camera);

  // to john: temporary solution to render single particles
  getObjectManager<TCompParticles>()->forEach([](TCompParticles* particles) {
    particles->render();
    });

  CTexture* curr_output = rt_final;

  // -- Now starting the final render presentation
  CEntity* e_camera = CEngine::get().getRender().getActiveCamera();

  TCompBloom* c_bloom = e_camera->get<TCompBloom>();
  if (c_bloom)
    c_bloom->generateHighlights(curr_output);

  TCompBlur* c_blur = e_camera->get<TCompBlur>();
  if (c_blur) {
    curr_output = c_blur->apply(curr_output);

    TCompFocus* c_focus = e_camera->get<TCompFocus>();
    if (c_focus)
      curr_output = c_focus->apply(rt_final, curr_output);
  }

  TCompEdgeDetector* c_edge_detector = e_camera->get<TCompEdgeDetector>();
  if (c_edge_detector)
    curr_output = c_edge_detector->apply(curr_output);
  // ----

  Render.activateBackBuffer();

  // HDR a ToneMapped+Gamma+LDR
  drawFullScreenQuad("presentation.pipeline", curr_output);

  RenderManager.renderAll(eRenderChannel::RC_UI, h_camera);

  // Give an opportunity to all the modules to render something
  CModuleManager& modules = CEngine::get().getModuleManager();

  modules.render();
  modules.renderDebug();
  modules.renderUI();
  modules.renderUIDebug();

  activateMainCamera();
  modules.renderInMenu();

  ImGuiEndFrame();
  Render.swapFrames();
}

void CRenderModule::stop() {
  rt_final->destroy();
  cte_camera.destroy();
  cte_world.destroy();
  cte_light.destroy();
  cte_object.destroy();
  cte_ui.destroy();
  cte_blur.destroy();
  cte_particle.destroy();
  ImGuiDestroy();
  deferred_renderer.destroy();
}

