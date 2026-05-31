import { defineStore } from 'pinia'
import { ref, computed } from 'vue'
import authApi from '../api/auth.js'

export const useAuthStore = defineStore('auth', () => {
  const token = ref(localStorage.getItem('token') || '')
  const user = ref(JSON.parse(localStorage.getItem('user') || 'null'))

  const isLoggedIn = computed(() => !!token.value)

  function setAuth(data) {
    token.value = data.token
    user.value = data.user
    localStorage.setItem('token', data.token)
    localStorage.setItem('user', JSON.stringify(data.user))
  }

  function clearAuth() {
    token.value = ''
    user.value = null
    localStorage.removeItem('token')
    localStorage.removeItem('user')
  }

  async function login(email, password) {
    const res = await authApi.login(email, password)
    if (res.data.code === 200) {
      setAuth(res.data.data)
    }
    return res.data
  }

  async function register(email, password, username) {
    const res = await authApi.register(email, password, username)
    if (res.data.code === 200) {
      setAuth(res.data.data)
    }
    return res.data
  }

  async function forgotPassword(email, newPassword) {
    const res = await authApi.forgotPassword(email, newPassword)
    return res.data
  }

  async function logout() {
    try {
      await authApi.logout()
    } finally {
      clearAuth()
    }
  }

  return {
    token,
    user,
    isLoggedIn,
    setAuth,
    clearAuth,
    login,
    register,
    forgotPassword,
    logout,
  }
})
