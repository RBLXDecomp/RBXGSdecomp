#pragma once
#include "util/Debug.h"

/*
class RBX::IStage { // Size=0xc
  private: RBX::IStage* upstream;
  private: RBX::IStage* downstream;
  
  public: void IStage(const RBX::IStage&);
  public: void IStage(RBX::IStage*, RBX::IStage*);
  public: virtual ~IStage();
  public: RBX::IStage* getUpstream();
  public: RBX::IStage* getDownstream();
  public: virtual RBX::IStage::StageType getStageType();
  public: RBX::IStage* findStage(RBX::IStage::StageType);
  public: virtual void stepWorld(int32_t, int32_t, bool);
  public: virtual RBX::Kernel* getKernel();
  public: RBX::IStage& operator=(const RBX::IStage&);
  public: virtual void* __vecDelDtor(uint32_t);
};
*/

namespace RBX {
	class Kernel;

	class __declspec(novtable) IStage {
		private:
			IStage *upstream;
			IStage *downstream;
		public:
			enum StageType : unsigned int{
				JOINT_STAGE = 0x0,
				CLUMP_STAGE = 0x1,
				TREE_STAGE = 0x2,
				ASSEMBLY_STAGE = 0x3,
				COLLISION_STAGE = 0x4,
				SLEEP_STAGE = 0x5,
				SEPARATE_STAGE = 0x6,
				SIMJOB_STAGE = 0x7,
				KERNEL_STAGE = 0x8,
			};
			IStage(IStage* upstream, IStage* downstream):upstream(upstream),downstream(downstream) {}

			virtual ~IStage() { 
				if (downstream)
					delete downstream; 
			}

			virtual StageType getStageType() = 0;
			IStage* findStage(StageType stageType)
			{
				IStage* idownstream = this;
				while (idownstream->getStageType() != stageType)
					idownstream = idownstream->downstream;
				return idownstream;
			}
			virtual void stepWorld(int worldStepId, int uiStepId, bool throttling) //not checked if matching
			{
				RBXASSERT(downstream);
				downstream->stepWorld(worldStepId, uiStepId, throttling);
			}
			virtual Kernel* getKernel(){
				RBXASSERT(downstream);
				return downstream->getKernel(); 
			};
			RBX::IStage* getDownstream() {return downstream;};
			RBX::IStage* getUpstream() {return upstream;};
	};
}

