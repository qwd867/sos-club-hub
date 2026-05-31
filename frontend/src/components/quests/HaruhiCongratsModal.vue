<template>
  <Teleport to="body">
    <Transition name="modal">
      <div v-if="modelValue" class="congrats-overlay" @click.self="close">
        <div class="congrats-modal">
          <div class="modal-glow"></div>
          <div class="modal-content">
            <div class="haruhi-avatar">
              <img src="/characters/haruhi.jpg" alt="haruhi" />
            </div>
            <h2 class="modal-title">今天也是充满干劲的一天！</h2>
            <p class="modal-subtitle">春日团长对你今天的表现非常满意</p>
            <div class="bonus-info">
              <span class="bonus-label">今日全部完成</span>
              <span class="bonus-value">+{{ bonusPoints }} SOSpt</span>
            </div>
            <button class="close-btn" @click="close">
              明天继续！
            </button>
          </div>
        </div>
      </div>
    </Transition>
  </Teleport>
</template>

<script setup>
defineProps({
  modelValue: { type: Boolean, default: false },
  bonusPoints: { type: Number, default: 0 }
})

const emit = defineEmits(['update:modelValue'])

function close() {
  emit('update:modelValue', false)
}
</script>

<style scoped>
.congrats-overlay {
  position: fixed;
  inset: 0;
  z-index: 2001;
  display: flex;
  align-items: center;
  justify-content: center;
  background: rgba(0, 0, 0, 0.5);
}

.congrats-modal {
  position: relative;
  width: 90%;
  max-width: 380px;
  border-radius: 24px;
  background: linear-gradient(135deg, #fff8e0, #fff0f5);
  overflow: hidden;
  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.2);
}

.modal-glow {
  position: absolute;
  inset: -2px;
  border-radius: 26px;
  background: linear-gradient(135deg, #FFD700, #FF69B4, #FFD700);
  background-size: 300% 300%;
  animation: glowRotate 3s linear infinite;
  z-index: -1;
  opacity: 0.7;
}

@keyframes glowRotate {
  0% { background-position: 0% 50%; }
  50% { background-position: 100% 50%; }
  100% { background-position: 0% 50%; }
}

.modal-content {
  position: relative;
  padding: 36px 28px 28px;
  text-align: center;
  background: rgba(255, 255, 255, 0.9);
  border-radius: 24px;
  margin: 2px;
}

.haruhi-avatar {
  width: 90px;
  height: 90px;
  border-radius: 50%;
  overflow: hidden;
  margin: 0 auto 16px;
  border: 3px solid #FFD700;
  box-shadow: 0 4px 20px rgba(255, 215, 0, 0.4);
  animation: avatarBounce 0.6s ease;
}

.haruhi-avatar img {
  width: 100%;
  height: 100%;
  object-fit: cover;
}

@keyframes avatarBounce {
  0% { transform: scale(0); }
  50% { transform: scale(1.15); }
  100% { transform: scale(1); }
}

.modal-title {
  font-size: 22px;
  font-weight: 800;
  color: #1f2937;
  margin: 0 0 8px;
}

.modal-subtitle {
  font-size: 14px;
  color: #6b7280;
  margin: 0 0 24px;
}

.bonus-info {
  background: linear-gradient(135deg, rgba(255, 215, 0, 0.15), rgba(255, 105, 180, 0.1));
  border-radius: 16px;
  padding: 16px;
  margin-bottom: 24px;
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.bonus-label {
  font-size: 12px;
  font-weight: 600;
  color: #b45309;
  letter-spacing: 1px;
}

.bonus-value {
  font-size: 28px;
  font-weight: 800;
  background: linear-gradient(135deg, #f59e0b, #ec4899);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
}

.close-btn {
  width: 100%;
  padding: 14px;
  border: none;
  border-radius: 14px;
  background: linear-gradient(135deg, #FFD700, #f59e0b);
  color: #fff;
  font-size: 16px;
  font-weight: 700;
  cursor: pointer;
  transition: all 0.2s;
  box-shadow: 0 4px 15px rgba(245, 158, 11, 0.3);
}

.close-btn:hover {
  transform: translateY(-2px);
  box-shadow: 0 6px 20px rgba(245, 158, 11, 0.4);
}

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
