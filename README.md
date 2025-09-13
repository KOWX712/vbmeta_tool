# vbmeta tool

a cli tool to calculate vbmeta digest (android verifiedBootHash)

## Usage

```sh
chmod +x vbmeta_tool
./vbmeta_tool

> digest:94d3fa399ea90a87e0e1ad20db88031c496f73c17a1ac144847396cc55580366
> hash_alg:sha256
> size:2048
```

```sh
./vbmeta_tool digest
94d3fa399ea90a87e0e1ad20db88031c496f73c17a1ac144847396cc55580366
```

## Know issue

- Wrong value if vbmeta is self signed.
- No support for device that doesn't support `/dev/block/by-name/` or `/dev/block/bootdevice/by-name/`
