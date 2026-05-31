<template>
  <AuthLayout title="欢迎回来" subtitle="SOS团等候多时了">
    <form @submit.prevent="handleLogin" class="auth-form">
      <div class="input-group">
        <input v-model="email" type="email" id="login-email" required placeholder=" " />
        <label for="login-email">邮箱</label>
        <div class="input-line"></div>
      </div>

      <div class="input-group">
        <input v-model="password" type="password" id="login-pwd" required placeholder=" " />
        <label for="login-pwd">密码</label>
        <div class="input-line"></div>
      </div>

      <transition name="fade">
        <div class="msg msg-error" v-if="error">
          <span class="msg-icon">!</span>
          {{ error }}
        </div>
      </transition>

      <button type="submit" class="auth-btn" :class="{ loading: isLoading }">
        <span v-if="!isLoading">登 录</span>
        <span v-else class="spinner"></span>
      </button>
    </form>

    <div class="auth-links">
      <router-link to="/register" class="auth-link">
        <span>+</span>
        注册账号
      </router-link>
      <router-link to="/forgot-password" class="auth-link">
        忘记密码？
      </router-link>
    </div>
  </AuthLayout>
</template>

<script setup>
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '../stores/auth.js'
import { useQuestStore } from '../stores/quests.js'
import AuthLayout from '../layouts/AuthLayout.vue'

const email = ref('')
const password = ref('')
const error = ref('')
const isLoading = ref(false)
const router = useRouter()
const authStore = useAuthStore()
const questStore = useQuestStore()

async function handleLogin() {
  error.value = ''
  isLoading.value = true
  try {
    const result = await authStore.login(email.value, password.value)
    if (result.code === 200) {
      // 触发任务进度
      questStore.reportProgress('legend_first_login', 1).catch(() => {})
      questStore.reportProgress('daily_checkin', 1).catch(() => {})
      router.push('/sos')
    } else {
      error.value = result.message
    }
  } catch (e) {
    error.value = e.response?.data?.message || '登录失败'
  } finally {
    isLoading.value = false
  }
}
</script>
