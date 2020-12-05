import {promisify} from 'util'
import { BlockNewParams, craEngineBlockNew, craEngineDelete, craEngineInit, craEngineNew, craEngineStop, craEngineBakeTexture } from './libcrankshaft-node-binding'

const engineStop = promisify(craEngineStop)
const engineInit = promisify(craEngineInit)
const engineBlockNew = promisify(craEngineBlockNew)
const engineBakeTexture = promisify(craEngineBakeTexture);
export default class Engine {
  public handle = craEngineNew()

  destroy() {
    craEngineDelete(this.handle)
  }

  async init() {
    return engineInit(this.handle)
  }

  async stop() {
    return engineStop(this.handle)
  }

  async blockNew(params: BlockNewParams) {
    return engineBlockNew(this.handle, params);
  }

  async bakeTexture() {
    return engineBakeTexture(this.handle);
  }
}
