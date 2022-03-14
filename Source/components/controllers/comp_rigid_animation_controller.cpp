#include "mcv_platform.h"
#include "entity/entity.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "resources/animation_data.h"

class CAnimationDataResourceType : public CResourceType
{
public:
	const char* getExtension(int idx) const { return ".anim"; }
	const char* getName() const { return "Animations"; }
	IResource* create(const std::string& name) const
	{
		TFileContext file_context(name);
		TCoreAnimationData* obj = new TCoreAnimationData();
		CFileDataProvider fdp(name.c_str());
		bool is_ok = obj->read(fdp);
		assert(is_ok);
		return obj;
	}
};

// -----------------------------------------
template<>
CResourceType* getClassResourceType<TCoreAnimationData>()
{
	static CAnimationDataResourceType factory;
	return &factory;
}


// -------------------------
struct TCompRigidAnimationController : public TCompBase
{

	struct TTrack {
		// What is being moved/animated???
		CHandle handle;
		// Detailed information to gather from the animation data to update handle
		const TCoreAnimationData::TCoreTrack* core_track;
		uint32_t next_key = 0;

		void rewind()
		{
			next_key = 0;
			apply(0.0f);
		}

		void apply(float current_time)
		{
			if (core_track->keys_type == TCoreAnimationData::TCoreTrack::eKeyType::TRANSFORM)
			{
				float ut = 0.0;

				if (current_time < core_track->min_time)
					ut = 0.0f;
				else if (current_time > core_track->max_time)
					ut = 1.0f;
				else
					ut = (current_time - core_track->min_time) / (core_track->max_time - core_track->min_time);

				// find the two nearest keys...
				float t = core_track->num_keys * ut;
				int prev_frame = (int)t;		// remove decimals
				int next_frame = prev_frame + 1;
				prev_frame = std::max(0, std::min(prev_frame, (int)core_track->num_keys - 1));
				next_frame = std::max(0, std::min(prev_frame + 1, (int)core_track->num_keys - 1));

				const auto& key0 = core_track->getKey<CTransform>(prev_frame);
				const auto& key1 = core_track->getKey<CTransform>(next_frame);
				float amount_of_1 = t - prev_frame;

				TCompTransform* c_trans = handle;
				assert(c_trans);
				*(CTransform*)c_trans = key0;
				c_trans->interpolateTo(key1, amount_of_1);
			}
			else if (core_track->keys_type == TCoreAnimationData::TCoreTrack::eKeyType::TIMED_NOTE)
			{
				// Only works when playing forward...
				while (next_key < core_track->num_keys)
				{
					const auto& note = core_track->getKey<TCoreAnimationData::TTimedNote>(next_key);
					if (current_time < note.time)
						break;
					dbg("  Animation event %d at time:%f Curr:%f Text: %s\n", next_key, note.time, current_time, note.getText(core_track->var_data));
					++next_key;
				}

			}
		}
	};

	const TCoreAnimationData* animation_data = nullptr;
	std::string           animation_src;
	std::vector< TTrack > tracks;
	float                 curr_time = 0.0f;
	float                 speed_factor = 1.0f;
	bool                  playing = false;
	bool                  autoloops = false;
	bool                  ping_pong = false;
	bool                  playing_forward = true;

	void load(const json& j, TEntityParseContext& ctx)
	{
		animation_src = j["src"];
		if(!animation_src.empty())
			animation_data = Resources.get(animation_src)->as<TCoreAnimationData>();
		speed_factor = j.value("speed_factor", speed_factor);
		autoloops = j.value("autoloops", autoloops);
	}

	void debugInMenu()
	{
		if (ImGui::Checkbox("Playing", &playing))
		{
			if (playing)
				start();
		}
		ImGui::Checkbox("Forward", &playing_forward);
		ImGui::Checkbox("Auto loops", &autoloops);
		if(autoloops)
			ImGui::Checkbox("Ping Pong", &ping_pong);
		ImGui::DragFloat("Current Time", &curr_time, 0.01f, 0.0f, 10.f);
		ImGui::DragFloat("Speed Factor", &speed_factor, 0.01f, 0.0f, 5.f);
	}

	void assignTracksToSceneObjects() {
		// resolve the tracks from the animation data
		tracks.clear();
		for (auto& ct : animation_data->tracks)
		{
			TTrack t;

			// Find sphere01 in the scene. Might need to customize maybe to find my children or something...
			t.handle = getEntityByName(ct.obj_name);
			if (!t.handle.isValid())
				continue;

			CEntity* e_obj = t.handle;
			if (strcmp(ct.property_name, "transform") == 0)
			{
				t.handle = e_obj->get<TCompTransform>();
			}
			else if (strcmp(ct.property_name, "notes") == 0)
			{
				// Nothing special, notes sent to the entity
			}
			else
			{
				// Implement other track types.. Camera info?, blur info? events?
				fatal("Don't know to how to update property name %s\n", ct.property_name);
				continue;
			}
			t.core_track = &ct;
			tracks.push_back(t);
		}
	}

	void start()
	{
		animation_data = Resources.get(animation_src)->as<TCoreAnimationData>();
		assignTracksToSceneObjects();
		curr_time = animation_data->header.min_time;
		playing = true;
	}

	void updateCurrentTime(float delta_time)
	{
		curr_time += delta_time * speed_factor * (playing_forward ? 1.0f : -1.0f);
		// Never go beyond the time
		if (curr_time > animation_data->header.max_time)
			curr_time = animation_data->header.max_time;
		else if (curr_time < animation_data->header.min_time)
			curr_time = animation_data->header.min_time;
	}

	void onEndOfAnimation()
	{
		// loop? / change_direction? / disable
		if (autoloops)
		{
			if (ping_pong)
				playing_forward = !playing_forward;
			
			if( playing_forward )
				curr_time = animation_data->header.min_time;
			else
				curr_time = animation_data->header.max_time;

			for (auto& t : tracks)
				t.rewind();
		}
		else
			playing = false;
	}

	void update(float delta_time)
	{

		if (!playing)
			return;
		//{
		//	// This needs to be removed/changed...
		//	start();
		//}

		for (auto& t : tracks)
			t.apply(curr_time);

		if (playing_forward)
		{
			if (curr_time <= animation_data->header.max_time)
			{
				// We can only exit if we know for sure we have applied the max_time to all objects
				if (curr_time == animation_data->header.max_time)
					onEndOfAnimation();
				else
					updateCurrentTime(delta_time);
			}
		} else {
			if (curr_time >= animation_data->header.min_time)
			{
				if (curr_time == animation_data->header.min_time)
					onEndOfAnimation();
				else
					updateCurrentTime(delta_time);
			}
		}
	}

};


// Culling
// 000000000011111000000111100000000000	TCompCulling.culling_bits
// 000000000000111111111110000000000000 TCompPotencialVisiblity.bits_per_zone
//  AND
// 000000000000111000000110000000000000

DECL_OBJ_MANAGER("animator_controller", TCompRigidAnimationController)

