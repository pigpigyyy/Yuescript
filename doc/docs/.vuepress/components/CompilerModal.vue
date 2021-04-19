<template>
	<modal name="compiler" @before-open="prepare()" height="auto" width="80%" scrollable>
		<yue-compiler :text="content" compileronly displayonly/>
	</modal>
</template>

<script>
	import Vue from 'vue';
	import VModal from 'vue-js-modal/dist/ssr.nocss';
	import 'vue-js-modal/dist/styles.css';
	Vue.use(VModal);
	import YueCompiler from './YueCompiler.vue';

	export default {
		components: {
			YueCompiler,
		},
		mounted () {
		},
		data() {
			return {
				content: "",
			};
		},
		methods: {
			prepare() {
				this.$data.content = window.yueCodes;
			},
		},
	}
</script>

