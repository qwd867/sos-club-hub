<template>
  <div
    class="quest-card"
    :class="[
      typeClass,
      statusClass,
      { 'is-new': isNewlyUnlocked, 'is-claimable': isClaimable }
    ]"
  >
    <div class="card-header">
      <span class="type-badge">{{ typeLabel }}</span>
      <span v-if="isNewlyUnlocked" class="new-badge">NEW</span>
      <span v-if="isClaimable" class="claim-badge">!</span>
    </div>

    <h3 class="quest-name">{{ quest.name }}</h3>
    <p class="quest-desc">{{ quest.description }}</p>

    <div v-if="quest.brainstorm_title" class="brainstorm-title">
      {{ quest.brainstorm_title }}
    </div>

    <div class="quest-progress" v-if="showProgress">
      <div class="progress-bar">
        <div
          class="progress-fill"
          :style="{ width: progressPercent + '%' }"
        ></div>
      </div>
      <span class="progress-text">{{ quest.current_progress }} / {{ quest.target_progress }}</span>
    </div>

    <div class="quest-footer">
      <div class="reward">
        <span class="reward-icon">SOS</span>
        <span class="reward-value">+{{ quest.points_reward }}</span>
      </div>
      <button
        v-if="isClaimable"
        class="claim-btn"
        @click.stop="$emit('claim', quest.id)"
      >
        领取
      </button>
      <button
        v-else-if="isClaimed"
        class="share-btn"
        @click.stop="$emit('share', quest)"
      >
        分享
      </button>
      <span v-else-if="isLocked" class="status-text locked">未解锁</span>
      <span v-else class="status-text">进行中</span>
    </div>
  </div>
</template>

<script setup>
import { computed } from 'vue'

const props = defineProps({
  quest: { type: Object, required: true }
})

defineEmits(['claim', 'share'])

const typeClass = computed(() => `type-${props.quest.type}`)
const statusClass = computed(() => `status-${props.quest.status}`)

const typeLabel = computed(() => {
  const map = { daily: '每日', weekly: '周常', legendary: '传说' }
  return map[props.quest.type] || props.quest.type
})

const isNewlyUnlocked = computed(() => props.quest.status === 'available' && props.quest.type === 'legendary')
const isClaimable = computed(() => props.quest.status === 'completed')
const isClaimed = computed(() => props.quest.status === 'claimed')
const isLocked = computed(() => props.quest.status === 'locked')
const showProgress = computed(() =>
  props.quest.status !== 'locked' &&
  props.quest.status !== 'available' &&
  props.quest.target_progress > 1
)

const progressPercent = computed(() => {
  if (!props.quest.target_progress) return 0
  return Math.min(100, (props.quest.current_progress / props.quest.target_progress) * 100)
})
</script>

<style scoped>
.quest-card {
  position: relative;
  padding: 20px;
  border-radius: 16px;
  background: rgba(255, 255, 255, 0.85);
  border: 2px solid transparent;
  transition: all 0.3s ease;
  cursor: default;
  display: flex;
  flex-direction: column;
  gap: 10px;
}

.quest-card:hover {
  transform: translateY(-4px);
  box-shadow: 0 12px 30px rgba(0, 0, 0, 0.1);
}

/* 类型颜色 */
.type-daily {
  border-color: rgba(59, 130, 246, 0.3);
  background: linear-gradient(135deg, rgba(239, 246, 255, 0.9), rgba(255, 255, 255, 0.9));
}
.type-weekly {
  border-color: rgba(147, 51, 234, 0.3);
  background: linear-gradient(135deg, rgba(250, 245, 255, 0.9), rgba(255, 255, 255, 0.9));
}
.type-legendary {
  border-color: rgba(234, 179, 8, 0.3);
  background: linear-gradient(135deg, rgba(254, 252, 232, 0.9), rgba(255, 255, 255, 0.9));
}

/* 状态样式 */
.status-claimed {
  opacity: 0.6;
  filter: grayscale(0.5);
}
.status-locked {
  opacity: 0.5;
  filter: grayscale(0.8);
}
.status-completed {
  border-color: rgba(234, 179, 8, 0.5);
  box-shadow: 0 0 20px rgba(234, 179, 8, 0.15);
}

/* 可领取闪烁 */
.is-claimable {
  animation: claimPulse 2s ease-in-out infinite;
}

@keyframes claimPulse {
  0%, 100% { box-shadow: 0 0 0 0 rgba(234, 179, 8, 0.3); }
  50% { box-shadow: 0 0 20px 4px rgba(234, 179, 8, 0.2); }
}

.card-header {
  display: flex;
  align-items: center;
  gap: 8px;
}

.type-badge {
  padding: 2px 10px;
  border-radius: 12px;
  font-size: 11px;
  font-weight: 700;
  letter-spacing: 1px;
}

.type-daily .type-badge {
  background: rgba(59, 130, 246, 0.15);
  color: #2563eb;
}
.type-weekly .type-badge {
  background: rgba(147, 51, 234, 0.15);
  color: #9333ea;
}
.type-legendary .type-badge {
  background: rgba(234, 179, 8, 0.15);
  color: #b45309;
}

.new-badge {
  padding: 2px 8px;
  border-radius: 10px;
  background: #ef4444;
  color: #fff;
  font-size: 10px;
  font-weight: 700;
  animation: newShake 1s ease-in-out infinite;
}

@keyframes newShake {
  0%, 100% { transform: rotate(-3deg); }
  50% { transform: rotate(3deg); }
}

.claim-badge {
  width: 22px;
  height: 22px;
  border-radius: 50%;
  background: linear-gradient(135deg, #fbbf24, #f59e0b);
  color: #fff;
  font-size: 14px;
  font-weight: 800;
  display: flex;
  align-items: center;
  justify-content: center;
  margin-left: auto;
  animation: badgeBounce 1s ease-in-out infinite;
}

@keyframes badgeBounce {
  0%, 100% { transform: scale(1); }
  50% { transform: scale(1.15); }
}

.quest-name {
  font-size: 17px;
  font-weight: 700;
  color: #1f2937;
  line-height: 1.4;
  margin: 0;
}

.quest-desc {
  font-size: 13px;
  color: #6b7280;
  line-height: 1.5;
  margin: 0;
}

.brainstorm-title {
  padding: 6px 12px;
  border-radius: 8px;
  background: linear-gradient(135deg, rgba(239, 68, 68, 0.1), rgba(249, 115, 22, 0.1));
  color: #dc2626;
  font-size: 12px;
  font-weight: 600;
  border-left: 3px solid #ef4444;
}

.quest-progress {
  display: flex;
  align-items: center;
  gap: 10px;
}

.progress-bar {
  flex: 1;
  height: 6px;
  border-radius: 3px;
  background: rgba(0, 0, 0, 0.06);
  overflow: hidden;
}

.progress-fill {
  height: 100%;
  border-radius: 3px;
  background: linear-gradient(90deg, #3b82f6, #8b5cf6);
  transition: width 0.5s ease;
}

.progress-text {
  font-size: 12px;
  color: #6b7280;
  font-weight: 600;
  white-space: nowrap;
}

.quest-footer {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-top: 4px;
  padding-top: 10px;
  border-top: 1px dashed rgba(0, 0, 0, 0.08);
}

.reward {
  display: flex;
  align-items: center;
  gap: 4px;
}

.reward-icon {
  font-size: 9px;
  font-weight: 800;
  color: #fff;
  background: linear-gradient(135deg, #f59e0b, #d97706);
  padding: 2px 5px;
  border-radius: 4px;
  letter-spacing: 0.5px;
}

.reward-value {
  font-size: 14px;
  font-weight: 700;
  color: #b45309;
}

.claim-btn {
  padding: 6px 18px;
  border: none;
  border-radius: 20px;
  background: linear-gradient(135deg, #fbbf24, #f59e0b);
  color: #fff;
  font-size: 13px;
  font-weight: 700;
  cursor: pointer;
  transition: all 0.2s;
  box-shadow: 0 2px 10px rgba(245, 158, 11, 0.3);
}

.claim-btn:hover {
  transform: scale(1.05);
  box-shadow: 0 4px 15px rgba(245, 158, 11, 0.5);
}

.share-btn {
  padding: 6px 14px;
  border: none;
  border-radius: 20px;
  background: linear-gradient(135deg, #3b82f6, #8b5cf6);
  color: #fff;
  font-size: 12px;
  font-weight: 700;
  cursor: pointer;
  transition: all 0.2s;
  box-shadow: 0 2px 8px rgba(59, 130, 246, 0.25);
}

.share-btn:hover {
  transform: scale(1.05);
  box-shadow: 0 4px 12px rgba(59, 130, 246, 0.4);
}

.status-text {
  font-size: 12px;
  font-weight: 600;
  color: #9ca3af;
}

.status-text.claimed {
  color: #22c55e;
}
.status-text.locked {
  color: #9ca3af;
}
</style>
