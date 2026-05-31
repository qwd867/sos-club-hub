<template>
  <div class="member-card" :style="cardStyle" @click="$emit('click')">
    <div class="card-glow" :style="glowStyle"></div>
    <div class="card-inner">
      <div class="avatar-wrap">
        <img :src="member.avatar" :alt="member.name" class="avatar" />
      </div>
      <div class="info">
        <h3 class="name">{{ member.name }}</h3>
        <span class="tag" :style="tagStyle">{{ member.tag }}</span>
        <p class="summary">{{ member.summary }}</p>
      </div>
    </div>
  </div>
</template>

<script setup>
import { computed } from 'vue'

const props = defineProps({
  member: { type: Object, required: true }
})

defineEmits(['click'])

const cardStyle = computed(() => ({
  '--member-color': props.member.color
}))

const glowStyle = computed(() => ({
  background: `radial-gradient(circle at 50% 0%, ${props.member.color}40, transparent 70%)`
}))

const tagStyle = computed(() => ({
  background: `${props.member.color}25`,
  color: props.member.color,
  border: `1px solid ${props.member.color}50`
}))
</script>

<style scoped>
.member-card {
  position: relative;
  border-radius: 20px;
  background: rgba(255, 255, 255, 0.55);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  border: 1px solid rgba(255, 255, 255, 0.6);
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.08);
  cursor: pointer;
  overflow: hidden;
  transition: all 0.4s cubic-bezier(0.34, 1.56, 0.64, 1);
}

.member-card:hover {
  transform: translateY(-10px) scale(1.02);
  box-shadow: 0 12px 40px rgba(0, 0, 0, 0.15), 0 0 30px var(--member-color)30;
  border-color: var(--member-color)60;
}

.card-glow {
  position: absolute;
  inset: 0;
  opacity: 0;
  transition: opacity 0.4s;
  pointer-events: none;
}

.member-card:hover .card-glow {
  opacity: 1;
}

.card-inner {
  position: relative;
  z-index: 1;
  padding: 24px;
  display: flex;
  flex-direction: column;
  align-items: center;
  text-align: center;
}

.avatar-wrap {
  width: 120px;
  height: 120px;
  border-radius: 50%;
  padding: 4px;
  background: linear-gradient(135deg, var(--member-color), #fff);
  margin-bottom: 16px;
  transition: transform 0.4s;
}

.member-card:hover .avatar-wrap {
  transform: scale(1.08);
}

.avatar {
  width: 100%;
  height: 100%;
  border-radius: 50%;
  object-fit: cover;
  border: 3px solid #fff;
}

.name {
  font-size: 20px;
  font-weight: 700;
  color: #2c2c2c;
  margin-bottom: 8px;
  letter-spacing: 1px;
}

.tag {
  display: inline-block;
  padding: 4px 12px;
  border-radius: 20px;
  font-size: 12px;
  font-weight: 600;
  margin-bottom: 12px;
}

.summary {
  font-size: 13px;
  color: #666;
  line-height: 1.6;
  display: -webkit-box;
  -webkit-line-clamp: 2;
  -webkit-box-orient: vertical;
  overflow: hidden;
}

@media (max-width: 640px) {
  .avatar-wrap {
    width: 90px;
    height: 90px;
  }
  .card-inner {
    padding: 16px;
  }
  .name {
    font-size: 16px;
  }
}
</style>
