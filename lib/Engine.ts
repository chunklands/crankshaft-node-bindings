import {promisify} from 'util'
import { craEngineDelete, craEngineInit, craEngineNew, craEngineStop } from './libcrankshaft-node-binding'

const engineStop = promisify(craEngineStop)
const engineInit = promisify(craEngineInit)

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
}
