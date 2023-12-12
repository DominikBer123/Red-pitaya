
### Example web page
An example of a minimal application for creating a module for a web page in redpitaya

#### Structure

* index.html - main page of the module that starts when the application starts
* fpga.conf - path to fpg firmware, loaded automatically when module starts
* nginx.conf - nginx configuration file
* (js, info, img, css) - folders containing files for the site
* src - folder containing the source code on c ++, which is executed on redpitaya


#### Build

To build a project, execute the command:

```
make INSTALL_DIR=/opt/redpitaya
```

In the INSTALL_DIR parameter you need to specify the path to the header files redpitaya api. The result should be the file controllerhf.so.

#### Deploy

In order for the module to appear on the redpitaya web page. You need to create a directory аnd copy the data from the project there.

```
mkdir /opt/redpitaya/www/apps/example
cp -R * /opt/redpitaya/www/apps/example
```









