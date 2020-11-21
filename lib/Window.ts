import { craWindowLoadGL, craWindowNew, WindowHandle, WindowNewParams } from "./libcrankshaft";
import {promisify} from 'util'
import Engine from "./Engine";

const windowNew = promisify(craWindowNew)
const windowLoadGL = promisify(craWindowLoadGL);

export default class Window {

    static async create(engine: Engine, params: WindowNewParams) {
      const window = await windowNew(engine.handle, params)
      return new Window(engine, window);
    }

    constructor(private engine: Engine, public handle: WindowHandle) {
    }

    async loadGL() {
      await windowLoadGL(this.handle);
    }
}