# Particle-Training-Veelo

This repo is set up as a simple Particle firmware project.

Training concepts are provided as **three separate git branches**:

- `training/cloud-publish`: demonstrates `Particle.publish()`
- `training/cloud-functions`: demonstrates `Particle.function()`
- `training/serial-publishing`: demonstrates printing/logging over `Serial`

### How to use this repo (beginner-friendly)

- **Switch to a lesson branch**:

```powershell
git checkout training/cloud-publish
git checkout training/cloud-functions
git checkout training/serial-publishing
```

- **Where the code is**: `src/main.ino`
- **How to run**: open the project in **Particle Workbench**, select a device + target platform, then flash.

### Quick “what should I see?”

- **cloud publish**: device publishes `training/cloud_publish` every ~10 seconds (watch in Particle Console event stream)
- **cloud functions**: call functions `led` and `rate` from the Particle Console for your device
- **serial publishing**: open Serial Monitor at **115200** and watch JSON lines + try commands like `toggle`

