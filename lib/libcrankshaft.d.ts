declare namespace crankshaft {
  export type DoneCallback<T = void> = (err: Error, result: T) => void
  export type ErrorHandler = (status: number) => void

  const EngineSymbol: unique symbol
  export interface EngineHandle {[EngineSymbol]: typeof EngineSymbol}
  export function craEngineNew(): EngineHandle
  export function craEngineDelete(engine: EngineHandle): void

  export function craEngineInit(engine: EngineHandle, callback: DoneCallback): void
  export function craEngineStop(engine: EngineHandle, callback: DoneCallback): void

  export interface WindowNewParams {
    width: number
    height: number
    title: string
  }

  const WindowSymbol: unique symbol
  export interface WindowHandle {[WindowSymbol]: typeof WindowSymbol}
  export function craWindowNew(engine: EngineHandle, params: WindowNewParams, callback: DoneCallback<WindowHandle>): void
  export function craWindowDelete(window: WindowHandle, callback: DoneCallback): void
  export function craWindowLoadGL(window: WindowHandle, callback: DoneCallback): void
  export function craWindowOnClose(window: WindowHandle, callback: () => void): void
}

export = crankshaft