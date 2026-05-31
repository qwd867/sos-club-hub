<template>
  <Teleport to="body">
    <Transition name="modal">
      <div v-if="modelValue" class="claim-overlay" @click.self="close">
        <div class="claim-modal">
          <div class="modal-glow"></div>

          <div class="modal-content">
            <div class="success-icon">
              <span class="icon-star">✦</span>
            </div>

            <h2 class="modal-title">任务完成！</h2>
            <p class="modal-subtitle">春日团长认可了你的努力</p>

            <div class="points-animate">
              <span class="points-label">获得贡献值</span>
              <div class="points-value" :key="displayPoints">
                +{{ displayPoints }}
              </div>
            </div>

            <div class="modal-actions">
              <button class="share-btn" @click="handleShare">
                <span>📷</span> 分享
              </button>
              <button class="close-btn" @click="close">
                关闭
              </button>
            </div>
          </div>
        </div>
      </div>
    </Transition>
  </Teleport>
</template>

<script setup>
import { ref, watch } from 'vue'

const props = defineProps({
  modelValue: { type: Boolean, default: false },
  points: { type: Number, default: 0 }
})

const emit = defineEmits(['update:modelValue', 'share'])

const displayPoints = ref(0)

watch(() => props.modelValue, (val) => {
  if (val) {
    displayPoints.value = 0
    animatePoints()
  }
})

function animatePoints() {
  const target = props.points
  const duration = 800
  const start = performance.now()

  function tick(now) {
    const elapsed = now - start
    const progress = Math.min(elapsed / duration, 1)
    const eased = 1 - Math.pow(1 - progress, 3)
    displayPoints.value = Math.round(target * eased)
    if (progress < 1) {
      requestAnimationFrame(tick)
    }
  }
  requestAnimationFrame(tick)
}

function close() {
  emit('update:modelValue', false)
}

function handleShare() {
  emit('share')
}
</script>

<style scoped>
.claim-overlay {
  position: fixed;
  inset: 0;
  z-index: 2000;
  display: flex;
  align-items: center;
  justify-content: center;
  background: rgba(0, 0, 0, 0.5);
}

.claim-modal {
  position: relative;
  width: 90%;
  max-width: 380px;
  border-radius: 24px;
  background: linear-gradient(135deg, #fff8e0, #fff0f5, #f0f4ff);
  background-size: 200% 200%;
  animation: modalGradient 6s ease infinite;
  overflow: hidden;
  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.2);
}

@keyframes modalGradient {
  0%, 100% { background-position: 0% 50%; }
  50% { background-position: 100% 50%; }
}

.modal-glow {
  position: absolute;
  inset: -2px;
  border-radius: 26px;
  background: linear-gradient(135deg, #FFD700, #FF69B4, #9370DB, #00BFFF);
  background-size: 300% 300%;
  animation: glowRotate 4s linear infinite;
  z-index: -1;
  opacity: 0.6;
}

@keyframes glowRotate {
  0% { background-position: 0% 50%; }
  50% { background-position: 100% 50%; }
  100% { background-position: 0% 50%; }
}

.modal-content {
  position: relative;
  padding: 40px 32px 32px;
  text-align: center;
  background: rgba(255, 255, 255, 0.85);
  border-radius: 24px;
  margin: 2px;
}

.success-icon {
  width: 70px;
  height: 70px;
  border-radius: 50%;
  background: linear-gradient(135deg, #fbbf24, #f59e0b);
  display: flex;
  align-items: center;
  justify-content: center;
  margin: 0 auto 20px;
  box-shadow: 0 8px 30px rgba(245, 158, 11, 0.4);
  animation: iconPop 0.5s ease;
}

@keyframes iconPop {
  0% { transform: scale(0) rotate(-180deg); }
  60% { transform: scale(1.2) rotate(10deg); }
  100% { transform: scale(1) rotate(0); }
}

.icon-star {
  font-size: 32px;
  color: #fff;
}

.modal-title {
  font-size: 24px;
  font-weight: 800;
  color: #1f2937;
  margin: 0 0 6px;
}

.modal-subtitle {
  font-size: 14px;
  color: #6b7280;
  margin: 0 0 28px;
}

.points-animate {
  background: linear-gradient(135deg, rgba(251, 191, 36, 0.15), rgba(245, 158, 11, 0.1));
  border-radius: 16px;
  padding: 20px;
  margin-bottom: 24px;
  border: 1px solid rgba(245, 158, 11, 0.2);
}

.points-label {
  display: block;
  font-size: 12px;
  font-weight: 600;
  color: #b45309;
  margin-bottom: 4px;
  letter-spacing: 1px;
}

.points-value {
  font-size: 42px;
  font-weight: 800;
  background: linear-gradient(135deg, #f59e0b, #d97706);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  line-height: 1.2;
}

.modal-actions {
  display: flex;
  gap: 12px;
}

.share-btn,
.close-btn {
  flex: 1;
  padding: 12px;
  border: none;
  border-radius: 14px;
  font-size: 15px;
  font-weight: 700;
  cursor: pointer;
  transition: all 0.2s;
}

.share-btn {
  background: linear-gradient(135deg, #3b82f6, #8b5cf6);
  color: #fff;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 6px;
  box-shadow: 0 2px 10px rgba(59, 130, 246, 0.3);
}

.share-btn:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 15px rgba(59, 130, 246, 0.4);
}

.close-btn {
  background: rgba(0, 0, 0, 0.05);
  color: #6b7280;
}

.close-btn:hover {
  background: rgba(0, 0, 0, 0.08);
}

/* Transition */
.modal-enter-active,
.modal-leave-active {
  transition: all 0.3s ease;
}

.modal-enter-from,
.modal-leave-to {
  opacity: 0;
  transform: scale(0.9);
}
</style>
