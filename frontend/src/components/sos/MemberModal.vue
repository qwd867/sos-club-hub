<template>
  <Teleport to="body">
    <Transition name="modal">
      <div v-if="modelValue" class="modal-overlay" @click.self="close">
        <div class="modal-content" :style="contentStyle">
          <button class="close-btn" @click="close">&times;</button>

          <div class="modal-body">
            <div class="modal-left">
              <div class="avatar-frame" :class="{ 'avatar-shake': isShaking }" :style="frameStyle">
                <img :src="member.avatar" :alt="member.name" class="modal-avatar" />
              </div>
              <h2 class="modal-name">{{ member.name }}</h2>
              <span class="modal-tag" :style="tagStyle">{{ member.tag }}</span>

              <div class="interact-area">
                <button
                  class="interact-btn"
                  :class="{ clicked: interactClicked, 'combo-glow': comboCount >= 3 }"
                  :style="interactBtnStyle"
                  @click="handleInteract"
                >
                  <span v-if="comboCount >= 2" class="combo-mark">x{{ comboCount }}</span>
                  <span v-else>与TA互动</span>
                </button>
                <div v-if="comboCount >= 3" class="combo-rating" :class="ratingClass">
                  {{ comboRating }}
                </div>
              </div>
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

        <!-- 浮动文字层（在 modal-content 内部） -->
        <div class="float-layer">
          <TransitionGroup name="float">
            <div
              v-for="item in floatingTexts"
              :key="item.id"
              class="float-text"
              :style="item.style"
            >
              {{ item.text }}
            </div>
          </TransitionGroup>
        </div>
      </div>
    </Transition>
  </Teleport>
</template>

<script setup>
import { ref, computed, onMounted, onUnmounted } from 'vue'

const props = defineProps({
  modelValue: { type: Boolean, required: true },
  member: { type: Object, required: true }
})

const emit = defineEmits(['update:modelValue', 'interact'])

function close() {
  emit('update:modelValue', false)
}

const interactClicked = ref(false)
const comboCount = ref(0)
const comboTimer = ref(null)
const isShaking = ref(false)
const floatingTexts = ref([])
let floatId = 0

const characterLines = {
  haruhi: ['哈？你有兴趣了？', '这还差不多！', '再来一次！', 'SOS团需要你这样的人！', '哈哈哈，有趣！', '我就知道你与众不同！'],
  yuki: ['…', '资讯操作。', '确认。', '了解。', '…（微微点头）', '数据已记录。'],
  mikuru: ['啊哇哇…', '这、这是禁止事项…', '请、请温柔一点…', '啊哇哇哇！', '我、我会努力的！', '呜…团长…'],
  koizumi: ['呵呵。', '这是个人兴趣。', '团长会高兴的。', '嘛，没办法的事。', '微笑。', '真有意思呢。'],
  kyon: ['喂，别闹了。', '我又要吐槽了吗…', '长门，拜托了。', '哈？你在干什么啊。', '总之先吐槽吧。', '真是的…'],
  tsuruya: ['哇哈哈！', '鹤屋家会赞助的！', '太有趣了！', '哇哈哈哈！', '来一杯茶吗？', '钱不是问题！'],
  sasaki: ['有趣。', '你和凉宫同学…', '呵呵。', '这是很有趣的现象。', '我只是在观察。', '世界线的分歧…']
}

const comboRating = computed(() => {
  if (comboCount.value >= 10) return 'PERFECT!!!'
  if (comboCount.value >= 7) return 'GREAT!!'
  if (comboCount.value >= 5) return 'COOL!'
  if (comboCount.value >= 3) return 'NICE!'
  return ''
})

const ratingClass = computed(() => {
  if (comboCount.value >= 10) return 'rating-perfect'
  if (comboCount.value >= 7) return 'rating-great'
  if (comboCount.value >= 5) return 'rating-cool'
  return 'rating-nice'
})

function spawnFloatText(text) {
  const id = ++floatId
  const angle = (Math.random() - 0.5) * 40 // -20 ~ 20 deg
  // 在 modal-left 区域（左侧 0~35%）随机分布，避免与右侧详情文字重叠
  const startX = 5 + Math.random() * 25 // 5% ~ 30%
  const startY = 25 + Math.random() * 35 // 25% ~ 60%（垂直方向覆盖头像+按钮区域）
  const size = 14 + Math.min(comboCount.value * 1.5, 12)
  const color = props.member.color

  floatingTexts.value.push({
    id,
    text,
    style: {
      left: `${startX}%`,
      top: `${startY}%`,
      transform: `rotate(${angle}deg)`,
      fontSize: `${size}px`,
      color,
      textShadow: `0 0 8px ${color}60`
    }
  })

  setTimeout(() => {
    floatingTexts.value = floatingTexts.value.filter(f => f.id !== id)
  }, 1200)
}

function handleInteract() {
  // 按钮点击动画
  interactClicked.value = true
  setTimeout(() => { interactClicked.value = false }, 200)

  // 连击计数
  comboCount.value++
  if (comboTimer.value) clearTimeout(comboTimer.value)
  comboTimer.value = setTimeout(() => {
    comboCount.value = 0
  }, 1500)

  // 头像抖动（连击越高越猛）
  isShaking.value = true
  setTimeout(() => { isShaking.value = false }, 300 + Math.min(comboCount.value * 50, 300))

  // 角色台词
  const lines = characterLines[props.member.id] || ['…']
  const line = lines[Math.min(comboCount.value - 1, lines.length - 1)] || lines[lines.length - 1]
  spawnFloatText(line)

  // 连击评级额外特效文字
  if (comboCount.value === 3 || comboCount.value === 5 || comboCount.value === 7 || comboCount.value === 10) {
    setTimeout(() => spawnFloatText(comboRating.value), 150)
  }

  // 发送互动请求（防抖：连击中断后才真正发送，或者每3击发一次）
  emit('interact', props.member.id)
}

function onKeydown(e) {
  if (e.key === 'Escape' && props.modelValue) {
    close()
  }
}

onMounted(() => window.addEventListener('keydown', onKeydown))
onUnmounted(() => {
  window.removeEventListener('keydown', onKeydown)
  if (comboTimer.value) clearTimeout(comboTimer.value)
})

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

const interactBtnStyle = computed(() => ({
  background: `linear-gradient(135deg, ${props.member.color}, ${props.member.color}dd)`,
  boxShadow: `0 4px 15px ${props.member.color}40`
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
  transition: transform 0.1s;
}

.avatar-shake {
  animation: avatarShake 0.3s ease;
}

@keyframes avatarShake {
  0%, 100% { transform: translateX(0) rotate(0); }
  20% { transform: translateX(-6px) rotate(-3deg); }
  40% { transform: translateX(6px) rotate(3deg); }
  60% { transform: translateX(-4px) rotate(-2deg); }
  80% { transform: translateX(4px) rotate(2deg); }
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
  margin-bottom: 16px;
}

.interact-area {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 8px;
  min-height: 70px;
}

.interact-btn {
  padding: 10px 28px;
  border: none;
  border-radius: 24px;
  color: #fff;
  font-size: 14px;
  font-weight: 700;
  cursor: pointer;
  transition: all 0.15s;
  letter-spacing: 1px;
  position: relative;
  overflow: hidden;
}

.interact-btn:hover {
  transform: translateY(-2px) scale(1.05);
}

.interact-btn.clicked {
  animation: btnClick 0.2s ease;
}

.interact-btn.combo-glow {
  animation: comboGlow 0.6s ease-in-out infinite alternate;
}

@keyframes btnClick {
  0% { transform: scale(1); }
  40% { transform: scale(0.88); }
  100% { transform: scale(1); }
}

@keyframes comboGlow {
  0% { box-shadow: 0 4px 15px var(--member-color)40; }
  100% { box-shadow: 0 4px 25px var(--member-color)80, 0 0 40px var(--member-color)30; }
}

.combo-mark {
  font-size: 18px;
  font-weight: 800;
}

.combo-rating {
  font-size: 20px;
  font-weight: 900;
  letter-spacing: 2px;
  animation: ratingPop 0.4s cubic-bezier(0.34, 1.56, 0.64, 1);
  text-shadow: 0 2px 8px rgba(0,0,0,0.15);
}

.rating-nice { color: #3b82f6; }
.rating-cool { color: #8b5cf6; }
.rating-great { color: #f59e0b; }
.rating-perfect {
  color: #ef4444;
  animation: ratingPop 0.4s cubic-bezier(0.34, 1.56, 0.64, 1), perfectPulse 0.8s ease-in-out infinite;
}

@keyframes ratingPop {
  0% { transform: scale(0) rotate(-10deg); opacity: 0; }
  60% { transform: scale(1.3) rotate(5deg); opacity: 1; }
  100% { transform: scale(1) rotate(0); opacity: 1; }
}

@keyframes perfectPulse {
  0%, 100% { transform: scale(1); text-shadow: 0 2px 8px rgba(239, 68, 68, 0.3); }
  50% { transform: scale(1.1); text-shadow: 0 2px 16px rgba(239, 68, 68, 0.6); }
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

/* 浮动文字 */
.float-layer {
  position: absolute;
  inset: 0;
  pointer-events: none;
  z-index: 10;
  overflow: hidden;
  border-radius: 24px;
}

.float-text {
  position: absolute;
  font-weight: 800;
  white-space: nowrap;
  pointer-events: none;
}

.float-enter-active {
  transition: all 0.8s cubic-bezier(0.25, 1, 0.5, 1);
}

.float-leave-active {
  transition: all 0.5s ease;
}

.float-enter-from {
  opacity: 0;
  transform: translateY(20px) scale(0.5) !important;
}

.float-leave-to {
  opacity: 0;
  transform: translateY(-100px) scale(1.2) !important;
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
