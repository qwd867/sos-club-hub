<template>
  <canvas ref="canvas" class="confetti-canvas"></canvas>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue'

const canvas = ref(null)
let animId = null
let particles = []

const COLORS = ['#FFD700', '#FF69B4', '#9370DB', '#00BFFF', '#32CD32', '#FF4500', '#20B2AA']

function spawn(count = 60) {
  const c = canvas.value
  if (!c) return
  const ctx = c.getContext('2d')
  const w = c.width = window.innerWidth
  const h = c.height = window.innerHeight

  for (let i = 0; i < count; i++) {
    particles.push({
      x: w / 2,
      y: h / 2,
      vx: (Math.random() - 0.5) * 18,
      vy: (Math.random() - 1.2) * 18,
      size: 4 + Math.random() * 8,
      color: COLORS[Math.floor(Math.random() * COLORS.length)],
      rotation: Math.random() * 360,
      rotSpeed: (Math.random() - 0.5) * 10,
      gravity: 0.25 + Math.random() * 0.2,
      drag: 0.98,
      life: 1,
      decay: 0.008 + Math.random() * 0.012,
      shape: Math.random() > 0.5 ? 'rect' : 'circle'
    })
  }

  if (!animId) loop()
}

function loop() {
  const c = canvas.value
  if (!c) return
  const ctx = c.getContext('2d')
  const w = c.width
  const h = c.height

  ctx.clearRect(0, 0, w, h)

  particles = particles.filter(p => p.life > 0)

  for (const p of particles) {
    p.x += p.vx
    p.y += p.vy
    p.vy += p.gravity
    p.vx *= p.drag
    p.vy *= p.drag
    p.rotation += p.rotSpeed
    p.life -= p.decay

    ctx.save()
    ctx.translate(p.x, p.y)
    ctx.rotate((p.rotation * Math.PI) / 180)
    ctx.globalAlpha = Math.max(0, p.life)
    ctx.fillStyle = p.color

    if (p.shape === 'circle') {
      ctx.beginPath()
      ctx.arc(0, 0, p.size / 2, 0, Math.PI * 2)
      ctx.fill()
    } else {
      ctx.fillRect(-p.size / 2, -p.size / 4, p.size, p.size / 2)
    }

    ctx.restore()
  }

  if (particles.length > 0) {
    animId = requestAnimationFrame(loop)
  } else {
    animId = null
    ctx.clearRect(0, 0, w, h)
  }
}

function clear() {
  particles = []
  if (animId) {
    cancelAnimationFrame(animId)
    animId = null
  }
  const c = canvas.value
  if (c) {
    const ctx = c.getContext('2d')
    ctx.clearRect(0, 0, c.width, c.height)
  }
}

onMounted(() => {
  const c = canvas.value
  if (c) {
    c.width = window.innerWidth
    c.height = window.innerHeight
  }
})

onUnmounted(() => {
  if (animId) cancelAnimationFrame(animId)
})

defineExpose({ spawn, clear })
</script>

<style scoped>
.confetti-canvas {
  position: fixed;
  inset: 0;
  pointer-events: none;
  z-index: 9999;
}
</style>
