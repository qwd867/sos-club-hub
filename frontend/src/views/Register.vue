<template>
  <AuthLayout title="加入我们" subtitle="SOS团需要新鲜血液">
    <form @submit.prevent="handleRegister" class="auth-form">
      <div class="input-group">
        <input v-model="username" type="text" id="reg-user" required placeholder=" " />
        <label for="reg-user">用户名</label>
        <div class="input-line"></div>
      </div>

      <div class="input-group">
        <input v-model="email" type="email" id="reg-email" required placeholder=" " />
        <label for="reg-email">邮箱</label>
        <div class="input-line"></div>
      </div>

      <div class="input-group">
        <input v-model="password" type="password" id="reg-pwd" required placeholder=" " />
        <label for="reg-pwd">密码</label>
        <div class="input-line"></div>
      </div>

      <transition name="fade">
        <div class="msg msg-error" v-if="error">
          <span class="msg-icon">!</span>
          {{ error }}
        </div>
      </transition>

      <button type="submit" class="auth-btn" :class="{ loading: isLoading }">
        <span v-if="!isLoading">注 册</span>
        <span v-else class="spinner"></span>
      </button>
    </form>

    <div class="auth-links center">
      <router-link to="/login" class="auth-link">
        <span>←</span>
        已有账号？返回登录
      </router-link>
    </div>
  </AuthLayout>
</template>

<script setup>
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '../stores/auth.js'
import AuthLayout from '../layouts/AuthLayout.vue'

const username = ref('')
const email = ref('')
const password = ref('')
const error = ref('')
const isLoading = ref(false)
const router = useRouter()
const authStore = useAuthStore()

async function handleRegister() {
  error.value = ''
  isLoading.value = true
  try {
    const result = await authStore.register(email.value, password.value, username.value)
    if (result.code === 200) {
      router.push('/sos')
    } else {
      error.value = result.message
    }
  } catch (e) {
    error.value = e.response?.data?.message || '注册失败'
  } finally {
    isLoading.value = false
  }
}
</script>
