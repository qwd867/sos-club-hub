<template>
  <Teleport to="body">
    <Transition name="modal">
      <div v-if="modelValue" class="brain-overlay" @click.self="close">
        <div class="brain-modal">
          <div class="modal-glow"></div>
          <div class="modal-content">
            <div class="brain-header">
              <span class="brain-icon">🧠</span>
              <h2>团长的突发奇想</h2>
              <p>证明 1 + 1 = 3（完成一道简单数学题）</p>
            </div>

            <div class="math-section">
              <div class="math-problem">
                <span class="num">3</span>
                <span class="op">+</span>
                <span class="num">5</span>
                <span class="op">=</span>
                <span class="num">?</span>
              </div>
              <div class="math-options">
                <button
                  v-for="opt in options"
                  :key="opt"
                  class="option-btn"
                  :class="{ selected: selected === opt, correct: showResult && opt === correctAnswer, wrong: showResult && selected === opt && opt !== correctAnswer }"
                  @click="selectOption(opt)"
                  :disabled="showResult"
                >
                  {{ opt }}
                </button>
              </div>
            </div>

            <div v-if="showResult" class="result-section">
              <p v-if="selected === correctAnswer" class="result-correct">
                ✦ 回答正确！团长认可了你的智慧！
              </p>
              <p v-else class="result-wrong">
                不对哦～答案是 {{ correctAnswer }}，再试一次吧！
              </p>
            </div>

            <div class="modal-actions">
              <button v-if="!showResult" class="skip-btn" @click="close">
                跳过
              </button>
              <button v-else class="close-btn" @click="closeAndClaim">
                {{ selected === correctAnswer ? '领取奖励' : '关闭' }}
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
  modelValue: { type: Boolean, default: false }
})

const emit = defineEmits(['update:modelValue', 'success'])

const options = [6, 7, 8, 9]
const correctAnswer = 8
const selected = ref(null)
const showResult = ref(false)

watch(() => props.modelValue, (val) => {
  if (val) {
    selected.value = null
    showResult.value = false
  }
})

function selectOption(opt) {
  selected.value = opt
  showResult.value = true
}

function close() {
  emit('update:modelValue', false)
}

function closeAndClaim() {
  if (selected.value === correctAnswer) {
    emit('success')
  }
  close()
}
</script>

<style scoped>
.brain-overlay {
  position: fixed;
  inset: 0;
  z-index: 2000;
  display: flex;
  align-items: center;
  justify-content: center;
  background: rgba(0, 0, 0, 0.5);
}

.brain-modal {
  position: relative;
  width: 90%;
  max-width: 400px;
  border-radius: 24px;
  background: linear-gradient(135deg, #fef3c7, #fde68a);
  overflow: hidden;
  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.2);
}

.modal-glow {
  position: absolute;
  inset: -2px;
  border-radius: 26px;
  background: linear-gradient(135deg, #f59e0b, #ef4444, #f59e0b);
  background-size: 300% 300%;
  animation: glowRotate 3s linear infinite;
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
  padding: 32px 28px 28px;
  text-align: center;
  background: rgba(255, 255, 255, 0.92);
  border-radius: 24px;
  margin: 2px;
}

.brain-header {
  margin-bottom: 24px;
}

.brain-icon {
  font-size: 36px;
  display: block;
  margin-bottom: 8px;
}

.brain-header h2 {
  font-size: 20px;
  font-weight: 800;
  color: #1f2937;
  margin: 0 0 4px;
}

.brain-header p {
  font-size: 13px;
  color: #6b7280;
  margin: 0;
}

.math-problem {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 12px;
  margin-bottom: 24px;
  padding: 20px;
  background: linear-gradient(135deg, rgba(251, 191, 36, 0.1), rgba(245, 158, 11, 0.1));
  border-radius: 16px;
}

.num, .op {
  font-size: 32px;
  font-weight: 800;
  color: #1f2937;
}

.op {
  color: #f59e0b;
}

.math-options {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: 12px;
  margin-bottom: 20px;
}

.option-btn {
  padding: 14px;
  border: 2px solid rgba(0, 0, 0, 0.08);
  border-radius: 14px;
  background: rgba(255, 255, 255, 0.8);
  color: #374151;
  font-size: 20px;
  font-weight: 700;
  cursor: pointer;
  transition: all 0.2s;
}

.option-btn:hover:not(:disabled) {
  border-color: #f59e0b;
  background: rgba(251, 191, 36, 0.1);
  transform: translateY(-2px);
}

.option-btn.selected {
  border-color: #f59e0b;
  background: rgba(251, 191, 36, 0.2);
}

.option-btn.correct {
  border-color: #22c55e;
  background: rgba(34, 197, 94, 0.2);
  color: #16a34a;
}

.option-btn.wrong {
  border-color: #ef4444;
  background: rgba(239, 68, 68, 0.2);
  color: #dc2626;
}

.result-section {
  margin-bottom: 20px;
}

.result-correct {
  font-size: 15px;
  font-weight: 700;
  color: #16a34a;
  margin: 0;
}

.result-wrong {
  font-size: 14px;
  color: #dc2626;
  margin: 0;
}

.modal-actions {
  display: flex;
  justify-content: center;
}

.skip-btn,
.close-btn {
  padding: 12px 32px;
  border: none;
  border-radius: 14px;
  font-size: 15px;
  font-weight: 700;
  cursor: pointer;
  transition: all 0.2s;
}

.skip-btn {
  background: rgba(0, 0, 0, 0.05);
  color: #6b7280;
}

.skip-btn:hover {
  background: rgba(0, 0, 0, 0.08);
}

.close-btn {
  background: linear-gradient(135deg, #fbbf24, #f59e0b);
  color: #fff;
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
