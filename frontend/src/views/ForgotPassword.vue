<template>
  <AuthLayout title="重置密码" subtitle="别让团长等太久">
    <form @submit.prevent="handleForgotPassword" class="auth-form">
      <div class="input-group">
        <input v-model="email" type="email" id="fp-email" required placeholder=" " />
        <label for="fp-email">邮箱</label>
        <div class="input-line"></div>
      </div>

      <div class="input-group">
        <input v-model="newPassword" type="password" id="fp-pwd" required placeholder=" " />
        <label for="fp-pwd">新密码</label>
        <div class="input-line"></div>
      </div>

      <transition name="fade">
        <div class="msg msg-error" v-if="error">
          <span class="msg-icon">!</span>
          {{ error }}
        </div>
      </transition>

      <transition name="fade">
        <div class="msg msg-success" v-if="success">
          <span class="msg-icon">✓</span>
          {{ success }}
        </div>
      </transition>

      <button type="submit" class="auth-btn" :class="{ loading: isLoading }">
        <span v-if="!isLoading">重 置</span>
        <span v-else class="spinner"></span>
      </button>
    </form>

    <div class="auth-links center">
      <router-link to="/login" class="auth-link">
        <span>←</span>
        返回登录
      </router-link>
    </div>
  </AuthLayout>
</template>

<script setup>
import { ref } from 'vue'
import { useAuthStore } from '../stores/auth.js'
import AuthLayout from '../layouts/AuthLayout.vue'

const email = ref('')
const newPassword = ref('')
const error = ref('')
const success = ref('')
const isLoading = ref(false)
const authStore = useAuthStore()

async function handleForgotPassword() {
  error.value = ''
  success.value = ''
  isLoading.value = true
  try {
    const result = await authStore.forgotPassword(email.value, newPassword.value)
    if (result.code === 200) {
      success.value = result.data?.message || '密码重置成功'
    } else {
      error.value = result.message
    }
  } catch (e) {
    error.value = e.response?.data?.message || '重置失败'
  } finally {
    isLoading.value = false
  }
}
</script>
