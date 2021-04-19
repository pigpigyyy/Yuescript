<template>
	<div>
		<button class="button" @click="compile()">Compile</button>
		<div style="display: none;">
			<slot></slot>
		</div>
	</div>
</template>

<script>
	export default {
		methods: {
			compile() {
				const node = this.$el.children[1];
				const code = node.innerText;
				window.yueCodes = code;
				this.$modal.show('compiler');
			},
		},
	}
</script>

<style scoped>
	.button {
		border: none;
		display: inline-block;
		font-size: 16px;
		color: #fff;
		background-color: #b7ae8f;
		text-decoration: none;
		padding: .4rem 0.8rem;
		border-radius: 4px;
		transition: background-color .1s ease;
		box-sizing: border-box;
		border-bottom: 1px solid #aaa07b;
		margin-bottom: 1em;
	}
	.button:hover {
		background-color: #beb69a;
	}
	.button:focus,
	.button:active:focus,
	.button.active:focus,
	.button.focus,
	.button:active.focus {
		outline: none;
	}
</style>

