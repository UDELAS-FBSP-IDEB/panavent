# Panavent Project - An Open-Source Mechanical Ventilator.

This repository contains all the essential components for designing and implementing a mechanical ventilator. The central component is a gas mixer for mechanical ventilators, utilizing a multiloop and parallel PID controller system. The design and control system for the expiratory valve are also included, along with a C# application to manage the device's behavior, define various respiratory parameters, and display flow, volume, and pressure graphs.

The project provides a comprehensive solution, including hardware designs, control algorithms, 3D models, and complete source code for easy replication and further development.


## Project Structure

The repository is structured as follows:

- `/Electronics`: Contains electronic circuit schematics and hardware design files used in the gas mixer.
- `/3DModels`: 3D models and designs for the components used in the gas mixer.
- `/SourceCode`: The full code used for the control system, designed to run on the embedded platform.

## Features

- **Precise Gas Mixing**: Achieved using PID controllers in a multiloop and parallel configuration.
- **Open Source**: All designs, code, and models are provided as open-source materials.
- **Comprehensive**: Includes electronic design, mechanical components, and the full control algorithm.
- **Replicable**: All components can be replicated and adapted for various applications in medical settings.

## Requirements

- MPLAB X IDE and a PICkit programmer for executing the code of the microcontroller.
- Visual Studio 2017 or later for compiling and running the PC software.
- 3D printer for generating the physical components.
- Basic knowledge of PID control systems and electronics.
  
## License
This project is licensed under the MIT License, allowing for open-source distribution and adaptation. Please credit the original authors when using or adapting this work.

## Contact
For any questions or further information, feel free to reach out at luys.santana.0@udelas.ac.pa
