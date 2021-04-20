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
						<prism-editor class="my-editor" v-model="code" :highlight="highlighter" @input="codeChanged($event)" line-numbers :readonly="readonly"></prism-editor>
					</ClientOnly>
				</div>
				<div class="childR" style="height: 30em;">
					<ClientOnly>
						<prism-editor class="my-editor" v-model="compiled" :highlight="highlighter" @input="codeChanged($event)" readonly></prism-editor>
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
			highlighter(code) {
				return highlight(code, languages.moon);
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

	/*
	Name:       Base16 Atelier Sulphurpool Light
	Author:     Bram de Haan (http://atelierbram.github.io/syntax-highlighting/atelier-schemes/sulphurpool)

	Prism template by Bram de Haan (http://atelierbram.github.io/syntax-highlighting/prism/)
	Original Base16 color scheme by Chris Kempson (https://github.com/chriskempson/base16)

	*/
	code[class*="language-"],
	pre[class*="language-"] {
		font-family: Consolas, Menlo, Monaco, "Andale Mono WT", "Andale Mono", "Lucida Console", "Lucida Sans Typewriter", "DejaVu Sans Mono", "Bitstream Vera Sans Mono", "Liberation Mono", "Nimbus Mono L", "Courier New", Courier, monospace;
		font-size: 1em;
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
	}

	pre > code[class*="language-"] {
		font-size: 1em;
	}

	pre[class*="language-"]::-moz-selection, pre[class*="language-"] ::-moz-selection,
	code[class*="language-"]::-moz-selection, code[class*="language-"] ::-moz-selection {
		text-shadow: none;
		background: #dfe2f1;
	}

	pre[class*="language-"]::selection, pre[class*="language-"] ::selection,
	code[class*="language-"]::selection, code[class*="language-"] ::selection {
		text-shadow: none;
		background: #dfe2f1;
	}

	/* Code blocks */
	pre[class*="language-"] {
		padding: 1em;
		margin: .5em 0;
		overflow: auto;
	}

	/* Inline code */
	:not(pre) > code[class*="language-"] {
		padding: .1em;
		border-radius: .3em;
	}

	.token.comment,
	.token.prolog,
	.token.doctype,
	.token.cdata {
		color: #898ea4;
	}

	.token.punctuation {
		color: #5e6687;
	}

	.token.namespace {
		opacity: .7;
	}

	.token.operator,
	.token.boolean,
	.token.number {
		color: #c76b29;
	}

	.token.property {
		color: #c08b30;
	}

	.token.tag {
		color: #3d8fd1;
	}

	.token.string {
		color: #22a2c9;
	}

	.token.selector {
		color: #6679cc;
	}

	.token.attr-name {
		color: #c76b29;
	}

	.token.entity,
	.token.url,
	.language-css .token.string,
	.style .token.string {
		color: #22a2c9;
	}

	.token.attr-value,
	.token.keyword,
	.token.control,
	.token.directive,
	.token.unit {
		color: #ac9739;
	}

	.token.statement,
	.token.regex,
	.token.atrule {
		color: #22a2c9;
	}

	.token.placeholder,
	.token.variable {
		color: #3d8fd1;
	}

	.token.deleted {
		text-decoration: line-through;
	}

	.token.inserted {
		border-bottom: 1px dotted #202746;
		text-decoration: none;
	}

	.token.italic {
		font-style: italic;
	}

	.token.important,
	.token.bold {
		font-weight: bold;
	}

	.token.important {
		color: #c94922;
	}

	.token.entity {
		cursor: help;
	}

	pre > code.highlight {
		outline: 0.4em solid #c94922;
		outline-offset: .4em;
	}

	/* overrides color-values for the Line Numbers plugin
	 * http://prismjs.com/plugins/line-numbers/
	 */
	.line-numbers .line-numbers-rows {
		border-right-color: #dfe2f1;
	}

	.line-numbers-rows > span:before {
		color: #979db4;
	}

	/* overrides color-values for the Line Highlight plugin
	 * http://prismjs.com/plugins/line-highlight/
	 */
	.line-highlight {
		background: rgba(107, 115, 148, 0.2);
		background: -webkit-linear-gradient(left, rgba(107, 115, 148, 0.2) 70%, rgba(107, 115, 148, 0));
		background: linear-gradient(to right, rgba(107, 115, 148, 0.2) 70%, rgba(107, 115, 148, 0));
	}
	
</style>

