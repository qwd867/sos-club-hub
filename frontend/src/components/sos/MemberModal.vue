<template>
  <Teleport to="body">
    <Transition name="modal">
      <div v-if="modelValue" class="modal-overlay" @click.self="close">
        <div class="modal-content" :style="contentStyle">
          <button class="close-btn" @click="close">&times;</button>

          <div class="modal-body">
            <div class="modal-left">
              <div class="avatar-frame" :style="frameStyle">
                <img :src="member.avatar" :alt="member.name" class="modal-avatar" />
              </div>
              <h2 class="modal-name">{{ member.name }}</h2>
              <span class="modal-tag" :style="tagStyle">{{ member.tag }}</span>
            </div>

            <div class="modal-right">
              <div class="detail-section">
                <h4 :style="sectionTitleStyle">角色定位</h4>
                <p>{{ member.details.position }}</p>
              </div>
              <div class="detail-section">
                <h4 :style="sectionTitleStyle">团内职责</h4>
                <p>{{ member.details.duty }}</p>
              </div>
              <div class="detail-section">
                <h4 :style="sectionTitleStyle">经典台词</h4>
                <p class="quote">{{ member.details.quote }}</p>
              </div>
              <div class="detail-section">
                <h4 :style="sectionTitleStyle">特殊能力</h4>
                <p>{{ member.details.ability }}</p>
              </div>
              <div class="detail-section">
                <h4 :style="sectionTitleStyle">萌点</h4>
                <p>{{ member.details.moe }}</p>
              </div>
            </div>
          </div>
        </div>
      </div>
    </Transition>
  </Teleport>
</template>

<script setup>
import { computed, onMounted, onUnmounted } from 'vue'

const props = defineProps({
  modelValue: { type: Boolean, required: true },
  member: { type: Object, required: true }
})

const emit = defineEmits(['update:modelValue'])

function close() {
  emit('update:modelValue', false)
}

function onKeydown(e) {
  if (e.key === 'Escape' && props.modelValue) {
    close()
  }
}

onMounted(() => window.addEventListener('keydown', onKeydown))
onUnmounted(() => window.removeEventListener('keydown', onKeydown))

const contentStyle = computed(() => ({
  '--member-color': props.member.color
}))

const frameStyle = computed(() => ({
  background: `linear-gradient(135deg, ${props.member.color}, #fff)`
}))

const tagStyle = computed(() => ({
  background: `${props.member.color}25`,
  color: props.member.color,
  border: `1px solid ${props.member.color}50`
}))

const sectionTitleStyle = computed(() => ({
  color: props.member.color
}))
</script>

<style scoped>
.modal-overlay {
  position: fixed;
  inset: 0;
  z-index: 1000;
  display: flex;
  justify-content: center;
  align-items: center;
  padding: 20px;
  background: rgba(30, 30, 60, 0.55);
}

.modal-content {
  position: relative;
  width: 100%;
  max-width: 800px;
  max-height: 90vh;
  overflow-y: auto;
  border-radius: 24px;
  background: rgba(250, 250, 255, 0.92);
  border: 1px solid rgba(255, 255, 255, 0.9);
  box-shadow: 0 24px 60px rgba(0, 0, 0, 0.25), 0 0 40px var(--member-color)20;
  will-change: transform, opacity;
}

.close-btn {
  position: absolute;
  top: 16px;
  right: 20px;
  width: 36px;
  height: 36px;
  border: none;
  border-radius: 50%;
  background: rgba(0, 0, 0, 0.08);
  color: #666;
  font-size: 24px;
  line-height: 1;
  cursor: pointer;
  transition: all 0.3s;
  z-index: 10;
}

.close-btn:hover {
  background: rgba(0, 0, 0, 0.15);
  color: #333;
  transform: rotate(90deg);
}

.modal-body {
  display: grid;
  grid-template-columns: 280px 1fr;
  gap: 32px;
  padding: 40px;
}

.modal-left {
  display: flex;
  flex-direction: column;
  align-items: center;
  text-align: center;
}

.avatar-frame {
  width: 200px;
  height: 200px;
  border-radius: 50%;
  padding: 5px;
  margin-bottom: 20px;
}

.modal-avatar {
  width: 100%;
  height: 100%;
  border-radius: 50%;
  object-fit: cover;
  border: 4px solid #fff;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.15);
}

.modal-name {
  font-size: 26px;
  font-weight: 700;
  color: #2c2c2c;
  margin-bottom: 10px;
}

.modal-tag {
  display: inline-block;
  padding: 5px 14px;
  border-radius: 20px;
  font-size: 13px;
  font-weight: 600;
}

.modal-right {
  display: flex;
  flex-direction: column;
  gap: 18px;
}

.detail-section h4 {
  font-size: 14px;
  font-weight: 700;
  margin-bottom: 6px;
  letter-spacing: 1px;
}

.detail-section p {
  font-size: 14px;
  color: #555;
  line-height: 1.7;
}

.detail-section .quote {
  font-style: italic;
  color: #777;
  padding-left: 12px;
  border-left: 3px solid var(--member-color);
}

.modal-enter-active,
.modal-leave-active {
  transition: opacity 0.35s ease;
}

.modal-enter-from,
.modal-leave-to {
  opacity: 0;
}

.modal-enter-active .modal-content,
.modal-leave-active .modal-content {
  transition: transform 0.35s cubic-bezier(0.34, 1.56, 0.64, 1);
}

.modal-enter-from .modal-content,
.modal-leave-to .modal-content {
  transform: scale(0.9) translateY(20px);
}

@media (max-width: 720px) {
  .modal-body {
    grid-template-columns: 1fr;
    padding: 28px 20px;
    gap: 20px;
  }
  .avatar-frame {
    width: 140px;
    height: 140px;
  }
}
</style>
