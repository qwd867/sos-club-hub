import { createRouter, createWebHistory } from 'vue-router'
import { useAuthStore } from '../stores/auth.js'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      redirect: '/login'
    },
    {
      path: '/login',
      name: 'login',
      component: () => import('../views/Login.vue'),
    },
    {
      path: '/register',
      name: 'register',
      component: () => import('../views/Register.vue'),
    },
    {
      path: '/forgot-password',
      name: 'forgot-password',
      component: () => import('../views/ForgotPassword.vue'),
    },
    {
      path: '/sos',
      name: 'sos',
      component: () => import('../views/SosRecruit.vue'),
      meta: { requiresAuth: true }
    },
    {
      path: '/sos/quests',
      name: 'quests',
      component: () => import('../views/QuestBoard.vue'),
      meta: { requiresAuth: true }
    },
    {
      path: '/sos/shop',
      name: 'shop',
      component: () => import('../views/TsuruyaShop.vue'),
      meta: { requiresAuth: true }
    },
    {
      path: '/demo',
      redirect: '/sos'
    },
  ],
})

router.beforeEach((to, from, next) => {
  const authStore = useAuthStore()
  if (to.meta.requiresAuth && !authStore.isLoggedIn) {
    next('/login')
  } else {
    next()
  }
})

export default router
