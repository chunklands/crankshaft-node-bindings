const { default: Engine } = require('../build/Engine');
const { default: Window } = require('../build/Window');

(async () => {
  const engine = new Engine();
  await engine.init();

  const window = await Window.create(engine, {
    width: 1024,
    height: 768,
    title: "CRANKSHAFT"
  });

  await window.loadGL();

  window.onClose(async () => {
    await window.destroy();
  })

  // await window.destroy();

  // await engine.stop();
  // engine.destroy();
})()
