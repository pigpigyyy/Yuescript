<template>
		<div style="width: 100%; height: auto;">
			<div class="parent" style="background-color: #f5f7ff;">
				<div class="childL" style="height: 2.5em;">
					<div class="childTitle">Yuescript {{ info }}</div>
				</div>
				<div class="childR" style="height: 2.5em;">
					<div class="childTitle">Lua</div>
				</div>
				<div class="childL" ref='yueEditor' style="height: 30em;">
					<ClientOnly>
						<prism-editor class="my-editor" v-model="code" :highlight="highlighterYue" @input="codeChanged($event)" line-numbers :readonly="readonly"></prism-editor>
					</ClientOnly>
				</div>
				<div class="childR" style="height: 30em;">
					<ClientOnly>
						<prism-editor class="my-editor" v-model="compiled" :highlight="highlighterLua" @input="codeChanged($event)" readonly></prism-editor>
					</ClientOnly>
				</div>
			</div>
			<div v-if="!compileronly">
				<button class="button" @click="runCode()">Run!</button>
				<textarea class="resultArea" readonly>{{ result }}</textarea>
			</div>
		</div>
</template>

<script>
	import { PrismEditor } from 'vue-prism-editor';
	import 'vue-prism-editor/dist/prismeditor.min.css';
	import { highlight, languages } from 'prismjs/components/prism-core';
	import 'prismjs/components/prism-moonscript';
	import 'prismjs/components/prism-lua';

	export default {
		props: {
			compileronly: {
				type: Boolean,
				default: false,
			},
			displayonly: {
				type: Boolean,
				default: false,
			},
			text: {
				type: String,
				default: '',
			},
		},
		components: {
			PrismEditor,
		},
		data() {
			return {
				info: 'Loading',
				readonly: true,
				code: '',
				compiled: '',
				result: '',
			};
		},
		mounted () {
			if (this.text !== '') {
				this.$data.code = this.text;
				this.codeChanged(this.text);
			}
			const check = ((self)=> {
				return ()=> {
					if (window.yue) {
						self.$data.info = window.yue.version();
						self.$data.readonly = false;
						const editor = self.$refs.yueEditor;
						if (editor.children.length === 0) {
							setTimeout(check, 100);
							return;
						}
						if (this.$data.code !== '') {
							this.codeChanged(this.$data.code);
						}
						const textArea = editor.children[0].children[1].children[0];
						textArea.focus();
					} else {
						setTimeout(check, 100);
					}
				}
			})(this);
			check();
		},
		methods: {
			runCode() {
				if (window.yue && this.$data.compiled !== '') {
					let res = '';
					try {
						res = window.yue.exec(this.$data.code);
					} catch (err) {
						res = err;
					}
					this.$data.result = res;
				}
			},
			highlighterYue(code) {
				return highlight(code, languages.moon);
			},
			highlighterLua(code) {
				return highlight(code, languages.lua);
			},
			codeChanged(text) {
				if (window.yue) {
					let res = ['','compiler error, and please help opening an issue for this. Thanks a lot!'];
					try {
						res = window.yue.tolua(text, true, !this.displayonly, true);
						if (res[0] !== '') {
							this.$data.compiled = res[0];
						} else {
							this.$data.compiled = res[1];
						}
					} catch (error) {
						this.$data.compiled = res[1];
					}
				}
			},
		}
	}
</script>

<style scoped>
	.resultArea {
		float: left;
		margin-right: 1em;
		overflow: scroll;
		overflow-wrap: break-word;
		width: calc(100% - 120px);
		height: 55px;
		border-color: #b7ae8f;
		outline: none;
		resize: none;
		margin-top: 5px;
	}
	.childL {
		float: left;
		width: 50%;
		box-sizing: border-box;
		background-clip: content-box;
		background: #f5f7ff;
	}
	.childR {
		float: left;
		width: 50%;
		box-sizing: border-box;
		background-clip: content-box;
		background: #f5f7ff;
	}
	.childTitle {
		width: 100%;
		font-size: 1.2em;
		color: #b7ae8f;
		text-align: center;
		padding: 0.2em;
	}
	.button {
		float: right;
		border: none;
		display: inline-block;
		font-size: 1.2rem;
		color: #fff;
		background-color: #b7ae8f;
		text-decoration: none;
		padding: .8rem 1.6rem;
		border-radius: 4px;
		transition: background-color .1s ease;
		box-sizing: border-box;
		border-bottom: 1px solid #aaa07b;
		margin-top: 10px;
		margin-right: 5px;
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

	.my-editor {
		font-family: Consolas, Menlo, Monaco, "Andale Mono WT", "Andale Mono", "Lucida Console", "Lucida Sans Typewriter", "DejaVu Sans Mono", "Bitstream Vera Sans Mono", "Liberation Mono", "Nimbus Mono L", "Courier New", Courier, monospace;
		line-height: 1.375;
		direction: ltr;
		text-align: left;
		white-space: pre;
		word-spacing: normal;
		word-break: normal;
		-moz-tab-size: 4;
		-o-tab-size: 4;
		tab-size: 4;
		-webkit-hyphens: none;
		-moz-hyphens: none;
		-ms-hyphens: none;
		hyphens: none;
		background: #f5f7ff;
		color: #5e6687;
		font-size: 15px;
	}

	.my-editor >>> .prism-editor__textarea:focus {
		outline: none;
	}
</style>

