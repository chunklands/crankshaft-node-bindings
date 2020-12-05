import bindings from 'bindings'

export type DoneCallback<T = void> = (err: Error, result: T) => void
export type ErrorHandler = (status: number) => void

declare const EngineSymbol: unique symbol
export interface EngineHandle {[EngineSymbol]: typeof EngineSymbol}

export interface WindowNewParams {
  width: number
  height: number
  title: string
}

export interface RenderpipelineInitParams {
  gbufferVertexShader: string
  gbufferFragmentShader: string
  lightingVertexShader: string
  lightingFragmentShader: string
  selectblockVertexShader: string
  selectblockFragmentShader: string
  spriteVertexShader: string
  spriteFragmentShader: string
  textVertexShader: string
  textFragmentShader: string
}

export interface BlockNewParams {
  id: string
  opaque: boolean
  texture?: Buffer
  faces: {
    [face: string]: ArrayBuffer
  }
}

declare const WindowSymbol: unique symbol
export interface WindowHandle {[WindowSymbol]: typeof WindowSymbol}


export declare function craLogSetLevel(level: 0|1|2|3|4|5): void
export declare function craEngineNew(): EngineHandle
export declare function craEngineDelete(engine: EngineHandle): void
export declare function craEngineInit(engine: EngineHandle, callback: DoneCallback): void
export declare function craEngineStop(engine: EngineHandle, callback: DoneCallback): void
export declare function craEngineBlockNew(engine: EngineHandle, params: BlockNewParams, callback: DoneCallback): void
export declare function craEngineBakeTexture(engine: EngineHandle, callback: DoneCallback): void

export declare function craWindowNew(engine: EngineHandle, params: WindowNewParams, callback: DoneCallback<WindowHandle>): void
export declare function craWindowDelete(window: WindowHandle, callback: DoneCallback): void
export declare function craWindowLoadGL(window: WindowHandle, callback: DoneCallback): void
export declare function craWindowOnClose(window: WindowHandle, callback: () => void): void

export declare function craRenderpipelineInit(window: WindowHandle, param: RenderpipelineInitParams, callback: DoneCallback): void;

module.exports = bindings('libcrankshaft-node-binding.node')
